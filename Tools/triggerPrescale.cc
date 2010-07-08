#include <iostream>
#include <vector>
#include <utility>
#include <fstream>

#include "TH1F.h"
#include "TFile.h"
#include "TChain.h"

#include "../Toolbox/ProgressMonitor.h"
#include "DataFormats/interface/KMetadata.h"
#include "../RootTools/FileInterface.h"
#include "Toolbox/CmdLineSetup.h"
#include "RootTools/RunLumiReader.h"

bool abortProgram = false;

struct compRunLS {
	bool operator ()(std::pair<unsigned long,unsigned long> a, std::pair<unsigned long,unsigned long> b) const
	{
		if (a.first<b.first)
			return true;
		if (a.first == b.first)
			return a.second < b.second;
		return false;
	}
};

void zmumu(std::vector<std::string> filenames, std::vector<std::string> jsonFiles, std::vector<std::string> hlTrigger, std::string outputFilename)
{
	TChain * lumis_tree = new TChain("Lumis");

	lumis_tree->SetCacheSize(200000000);
	for (std::vector<std::string>::iterator it=filenames.begin(); it!=filenames.end();it++)
	{
		std::cout << "loading " << *it << "...\n";
		lumis_tree->Add((*it).c_str());
	}

	TBranch * b_lumimetadata;
	KLumiMetadata * m_lumimetadata = new KLumiMetadata();
	lumis_tree->SetBranchAddress("KLumiMetadata",&m_lumimetadata,&b_lumimetadata);
	lumis_tree->AddBranchToCache(b_lumimetadata);

	std::cout << "retrieving number of lumi sections...\n";
	Long64_t nentries_reco = lumis_tree->GetEntries();
	std::cout << nentries_reco << " lumi sections found\n";

	RunLumiSelector runLumiSelector("", 0, 0);
	for (std::vector<std::string>::iterator it = jsonFiles.begin(); it != jsonFiles.end(); it++)
		runLumiSelector.addJSONFile(*it);
	if (jsonFiles.size() == 0)
		runLumiSelector = RunLumiSelector("", 0, 1);

	std::map<std::pair<unsigned long,unsigned long>, std::vector<int>, compRunLS> prescalesTable;

	ProgressMonitor * timer = new ProgressMonitor(nentries_reco);

	for (unsigned int i=0;i<nentries_reco && !abortProgram;i++)
	{
		if (!timer->Update(1+i))
			break;

		lumis_tree->GetEntry(i);

		if (!runLumiSelector.accept(m_lumimetadata->nRun, m_lumimetadata->nLumi))
			continue;

		std::vector<int> prescales(hlTrigger.size());
		for (unsigned int idx1 = 0; idx1 < hlTrigger.size(); idx1++)
		{
			std::string hltName = hlTrigger[idx1];
			if ( find(m_lumimetadata->hltNames.begin(), m_lumimetadata->hltNames.end(), hltName) == m_lumimetadata->hltNames.end())
				continue;

			unsigned int idx2 = find(m_lumimetadata->hltNames.begin(), m_lumimetadata->hltNames.end(), hltName) - m_lumimetadata->hltNames.begin();

			prescales[idx1] = m_lumimetadata->hltPrescales[idx2];
		}

		std::pair<unsigned long,unsigned long> tmpID =std::make_pair(m_lumimetadata->nRun, m_lumimetadata->nLumi);
		prescalesTable[tmpID] = prescales;
	}
	//counter->SetBinContent(1, cnt);
	delete timer;

	std::pair<unsigned long,unsigned long> prevID;
	std::vector<int> prevPrescales(hlTrigger.size());
	for (std::map<std::pair<unsigned long,unsigned long>, std::vector<int>, compRunLS>::iterator it = prescalesTable.begin(); it != prescalesTable.end(); it++)
	{
		bool diff = false;
		for (unsigned int idx = 0; idx < hlTrigger.size() && !diff; idx++)
			if (prevPrescales[idx]!=(*it).second[idx])
				diff = true;
		if (diff)
		{
			std::cout << "\n";
			if (prevID.first == (*it).first.first)
				std::cout << "from ls " << prevID.second << " to ls " << (*it).first.second << " in run "<< prevID.first  << ":\n";
			else
				std::cout << "from run "<< prevID.first << " to run " << (*it).first.first << ":\n";
			for (unsigned int idx = 0; idx < hlTrigger.size(); idx++)
				if (prevPrescales[idx]!=(*it).second[idx])
				{
					//std::cout <<  "\t";
					std::cout.width ( 30 );
					std::cout << hlTrigger[idx];
					std::cout << "  ";
					std::cout.width ( 5 );
					std::cout << prevPrescales[idx];
					std::cout << " -> ";
					std::cout.width ( 5 );
					std::cout << (*it).second[idx];
					std::cout << "\n";
				}
			prevID = (*it).first;
			prevPrescales = (*it).second;
		}
	}
	std::cout << std::endl;
}

int main(int argc, char* argv[])
{
	CmdLineOptionSwitch<bool> optBatchMode('b', "batch", "Bool", false);
	CmdLineOptionValue<int> optVerbosity('v', "verbosity", "Integer", 1);
	CmdLineOptionValue<std::string> optOutputFile('o', "output", "String", "triggertest.root");
	CmdLineOptionVector<std::string> optJsonFiles('j',"json","String","");
	CmdLineOptionVector<std::string> optHLTs('t',"hlt","String","");

	std::vector<std::string> filenames = CmdLineBase::ParseArgs(argc, argv, OPT_Help | OPT_Version);
	CmdLineBase::Show("");

	//opts.outputFile = optOutputFile.Value();

	std::vector<std::string> hlTrigger = optHLTs.Value();
	if (hlTrigger.size()==0)
	{
		hlTrigger.push_back("HLT_L1Mu");
		hlTrigger.push_back("HLT_Mu3");
		hlTrigger.push_back("HLT_Mu5");
		hlTrigger.push_back("HLT_Mu9");
		hlTrigger.push_back("HLT_DoubleMu3");
		hlTrigger.push_back("HLT_Jet15U");
		hlTrigger.push_back("HLT_Jet30U");
		hlTrigger.push_back("HLT_Jet50U");
		hlTrigger.push_back("HLT_IsoMu3");
		hlTrigger.push_back("HLT_DiJetAve15U_8E29");
		hlTrigger.push_back("HLT_DiJetAve30U_8E29");//	hlTrigger.push_back("TestPath");
	}
	if (filenames.size())
	{
		if (optBatchMode)
				std::cout << "running in batch mode...\n";

		zmumu(filenames, optJsonFiles.Value(), hlTrigger, optOutputFile);
	}
	else
	{
		std::cout << "No input files specified. Quitting program.\n";
	}
	return 0;
}
