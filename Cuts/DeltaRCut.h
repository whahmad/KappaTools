#ifndef KDeltaRCut_h
#define KDeltaRCut_h

#include "BaseCut.h"
#include <Math/GenVector/VectorUtil.h>
#include "Kappa/DataFormats/interface/KBasic.h"
#include "Kappa/DataFormats/interface/KLorentzVector.h"

namespace KappaTools
{
	template <typename T1, typename T2>
	class DeltaRCut : public BaseCut
	{
	private:
		const T1 * obj1;
		const T2 * obj2;
		double dR_min;
		double dR_max;
	public:
		DeltaRCut();
		void setPointer(const T1 * tmpObj1, const T2 * tmpObj2);
		void setMinCut(double tmpdR_min);
		void setMaxCut(double tmpdR_max);
		bool getInternalDecision();

		double getDecisionValue();
	};
}

#endif
