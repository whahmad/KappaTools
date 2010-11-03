#ifndef KAPPA_FILEINTERFACE_H
#define KAPPA_FILEINTERFACE_H

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <list>

#include <Toolbox/IOHelper.h>
#include <Toolbox/ProgressMonitor.h>
#include <DataFormats/interface/Kappa.h>
#include "Directory.h"

typedef unsigned int run_id;
typedef unsigned int lumi_id;

struct FileInterface
{
	FileInterface(std::vector<std::string> files, bool shuffle = false, int verbose = 2);
	void SpeedupTree(long cache = 0);

	TChain eventdata;
	bool isMC;

	// Get event content of files
	template<typename T>
	T *Get(const std::string &name, const std::string altName = "");
	template<typename T>
	std::vector<std::string> GetNames(bool inherited = false);
	template<typename T>
	std::map<std::string, T*> GetAll(bool inherited = false);

	// Get event metadata objects
	KEventMetadata *GetEventMetadata();
	KGenEventMetadata *GetGenEventMetadata();
	void AssignEventPtr(KEventMetadata **meta_event, KGenEventMetadata **meta_event_gen = 0);

	// Get lumi metadata objects
	KLumiMetadata *GetLumiMetadata(run_id run, lumi_id lumi);
	KGenLumiMetadata *GetGenLumiMetadata(run_id run, lumi_id lumi);
	void AssignLumiPtr(run_id run, lumi_id lumi, KLumiMetadata **meta_lumi, KGenLumiMetadata **meta_lumi_gen = 0);

	// Get lumi list
	std::vector<std::pair<run_id, lumi_id> > GetRunLumis();

	// check if
	bool isCompatible(unsigned int minRun, unsigned int maxRun);
private:
	TChain lumidata;
	int verbosity;
	KEventMetadata *current_event;

	std::list<void*> vBranchHolder;
	std::map<std::pair<run_id, lumi_id>, KGenLumiMetadata> lumimap_mc;
	std::map<std::pair<run_id, lumi_id>, KLumiMetadata> lumimap_data;

	template<typename T>
	std::map<std::pair<run_id, lumi_id>, T> GetLumis();

	void *GetInternal(TChain &chain, const char *cname, const std::string &name, const std::string altName = "");
};

#include "FileInterface.hxx"

#endif
