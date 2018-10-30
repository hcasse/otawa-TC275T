#include <elm/avl/Set.h>
#include <otawa/parexegraph/GraphBBTime.h>
#include <otawa/parexegraph/ParExeGraph.h>
#include <otawa/proc/ProcessorPlugin.h>
#include <otawa/proc/Processor.h>
#include <otawa/branch/BranchBuilder.h>
#include <otawa/branch/features.h>
#include <otawa/cfg/features.h>
#include <otawa/loader/gliss.h>
#include <elm/avl/Map.h>
#include <otawa/data/clp/features.h>

#include <gel/gel.h>
#include <gel/gel_elf.h>

#define ELF32_ST_BIND(i) ((i) >> 4)
#define ELF32_ST_TYPE(i) ((i) & 0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

using namespace otawa;
namespace otawa { namespace tricore16 {


class CacheBlockFinder: public otawa::Processor {

public:
	static p::declare reg;
	CacheBlockFinder(void): otawa::Processor(reg), cache(0) { }

private:

	/**
	 * Get the binding of a symbol.
	 * @param infos		Information about the symbol.
	 * @return			Binding as a string.
	 */
	const char *get_binding(const gel_sym_info_t *infos) {
		static char buf[32];
		switch(ELF32_ST_BIND(infos->info)) {
		case STB_LOCAL: return "local";
		case STB_GLOBAL: return "global";
		case STB_WEAK: return "weak";
		default:
			sprintf(buf, "%d", ELF32_ST_BIND(infos->info));
			return buf;
		}
	}

	/**
	 * Get the type of a symbol.
	 * @param infos		Information about the symbol.
	 * @return			Type as a string.
	 */
	const char *get_type(const gel_sym_info_t *infos) {
		static char buf[32];
		switch(ELF32_ST_TYPE(infos->info)) {
		case STT_NOTYPE: return "notype";
		case STT_OBJECT: return "object";
		case STT_FUNC: return "func";
		case STT_SECTION: return "section";
		case STT_FILE: return "file";
		default:
			sprintf(buf, "%d", ELF32_ST_TYPE(infos->info));
			return buf;
		}
	}

	/**
	 * Get a section as a text.
	 * @param file		Current file.
	 * @param infos		Information about the symbol.
	 * @return			Section as a string.
	 */
	const char *get_section_index(gel_file_t *file, const gel_sym_info_t *infos) {
		static char buf[32];
		switch(infos->sect) {
		case SHN_UNDEF: return "undef";
		case SHN_ABS: return "abs";
		case SHN_COMMON: return "common";
		default: {
				gel_sect_info_t si;
				gel_sect_t *sect = gel_getsectbyidx(file, infos->sect);
				if(sect == NULL) {
					sprintf(buf, "%d", infos->sect);
					return buf;
				}
				gel_sect_infos(sect, &si);
				return si.name;
			}
		}
	}

protected:
	virtual void configure(const PropList& props) {
		otawa::Processor::configure(props);
	}

	virtual void processWorkSpace(WorkSpace *fw) {

		const hard::CacheConfiguration *conf = hard::CACHE_CONFIGURATION(fw);
		if(conf)
		cache = conf->instCache();
		if(!cache)
			throw ProcessorException(*this, "No cache in this platform.");

		elm::cout << "cache row = " << cache->rowCount() << endl;


		Vector<Pair<Address, String> >** allCBs  = new Vector<Pair<Address, String> >*[cache->rowCount()];
		for(int i = 0; i < cache->rowCount(); i++)
			allCBs[i] = new Vector<Pair<Address, String> >;


		const char *path;
		gel_file_t *file;
		int opt;
		gel_sym_iter_t iter;
		gel_sym_t *sym;

		elm::cout << fw->process()->program()->name().chars() << endl;

		file = gel_open(fw->process()->program()->name().chars(), "", 0);
		ASSERT(file);

		for(sym = gel_sym_first(&iter, file); sym; sym = gel_sym_next(&iter)) {
			/* get information */
			gel_sym_info_t infos;
			gel_sym_infos(sym, &infos);
			if(String(infos.name).length() == 0)
				continue;

			if(ELF32_ST_TYPE(infos.info) != STT_FUNC)
				continue;

			/* display information */
			elm::cout << hex(infos.vaddr) << " "
				<< infos.size << " "
				<< get_binding(&infos) << " "
				<< get_type(&infos) << " "
				<< get_section_index(file, &infos) << " "
				<< infos.name << endl;

			int num_lblocks =
				((infos.vaddr + infos.size + cache->blockMask()) >> cache->blockBits())
				- (infos.vaddr >> cache->blockBits());

			elm::cout << "\t" << "cache block = " << num_lblocks << endl;

			Address curr = cache->round(infos.vaddr);




			while(curr.offset() <= (infos.vaddr + infos.size - 1)) {
				hard::Cache::set_t set = cache->set(curr);
				ASSERT(set < cache->rowCount());
				elm::cout << "\t\tset " << set << " @ " << curr << endl;

				allCBs[set]->add(pair(curr, String(infos.name)));

				curr = Address(curr.offset() + cache->blockSize());
			}

//			Inst* inst = fw->findInstAt(Address(infos.vaddr));
//
//			while(inst && (inst->address().offset() < (infos.vaddr + infos.size)) {
//				elm::cout << inst << " @ " << inst->address() << endl;
//				inst = inst->nextInst();
//			}


			for(int i = 0; i < cache->rowCount(); i++) {
				elm::cout << "For set " << i << ": " << endl;
				Vector<Pair<Address, String> >* v = allCBs[i];
				for(Vector<Pair<Address, String> >::Iter vi(*v); vi; vi++)
					elm::cout << "\t" << (*vi).fst << "(" << (*vi).snd << ")" << endl;
				if(v->count() == 0)
					elm::cout << "\tempty" << endl;


			}


		}

		gel_close(file);





	}


private:
	const hard::Cache *cache;
};


p::declare CacheBlockFinder::reg = p::init("otawa::tricore16::CacheBlockFinder", Version(1, 0, 0))
		.base(otawa::Processor::reg)
		.maker<CacheBlockFinder>()
		.require(COLLECTED_CFG_FEATURE)
		.require(LOOP_INFO_FEATURE)
		.require(hard::CACHE_CONFIGURATION_FEATURE)
		//.provide(clp::CLP_ANALYSIS_FEATURE);
		;

}}


