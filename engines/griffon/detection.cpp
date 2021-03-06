/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "base/plugins.h"
#include "engines/advancedDetector.h"

#include "griffon/griffon.h"

static const PlainGameDescriptor griffonGames[] = {
	{"griffon", "The Griffon Legend"},
	{NULL, NULL}
};

namespace Griffon {
static const ADGameDescription gameDescriptions[] = {
	{
		"griffon",
		NULL,
		AD_ENTRY1s("objectdb.dat", "ec5371da28f01ccf88980b32d9de2232", 27754),
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE | ADGF_DROPPLATFORM,
		GUIO1(GUIO_NONE)
	},

	AD_TABLE_END_MARKER
};
}

class GriffonMetaEngine: public AdvancedMetaEngine {
public:
	GriffonMetaEngine() : AdvancedMetaEngine(Griffon::gameDescriptions, sizeof(ADGameDescription), griffonGames) {
	}

	virtual const char *getEngineId() const {
		return "griffon";
	}

	virtual const char *getName() const {
		return "Griffon Engine";
	}

	virtual int getMaximumSaveSlot() const { return 3; }

	virtual const char *getOriginalCopyright() const {
		return "The Griffon Legend (c) 2005 Syn9 (Daniel Kennedy)";
	}

	virtual bool hasFeature(MetaEngineFeature f) const;
	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;
};

bool GriffonMetaEngine::hasFeature(MetaEngineFeature f) const {
	return
		(f == kSupportsListSaves) ||
		(f == kSupportsDeleteSave) ||
		(f == kSavesSupportMetaInfo) ||
		(f == kSavesSupportThumbnail) ||
		(f == kSavesSupportCreationDate) ||
		(f == kSavesSupportPlayTime) ||
		(f == kSupportsLoadingDuringStartup) ||
		(f == kSavesUseExtendedFormat);
}

bool Griffon::GriffonEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL) ||
		(f == kSupportsLoadingDuringRuntime) ||
		(f == kSupportsSavingDuringRuntime);
}

bool GriffonMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	if (desc)
		*engine = new Griffon::GriffonEngine(syst);

	return desc != nullptr;
}

#if PLUGIN_ENABLED_DYNAMIC(GRIFFON)
REGISTER_PLUGIN_DYNAMIC(GRIFFON, PLUGIN_TYPE_ENGINE, GriffonMetaEngine);
#else
REGISTER_PLUGIN_STATIC(GRIFFON, PLUGIN_TYPE_ENGINE, GriffonMetaEngine);
#endif
