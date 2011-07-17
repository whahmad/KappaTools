#include "LSWatcher.h"

LSWatcher::LSWatcher()
	: changed(true), run(-1), lumi(-1)
{
}

bool LSWatcher::Changed(KEventMetadata *meta_event)
{
	if (meta_event)
	{
		changed = ((meta_event->nRun != run) || (meta_event->nLumi != lumi));
		if (changed)
		{
			run = meta_event->nRun;
			lumi = meta_event->nLumi;
		}
	}
	return changed;
}
