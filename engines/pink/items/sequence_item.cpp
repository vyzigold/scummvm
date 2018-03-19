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

#include <common/debug.h>
#include "sequence_item.h"
#include "../archive.h"
#include "sequence_item_leader.h"
#include "sequence_item_default_action.h"

namespace Pink {

void SequenceItem::deserialize(Archive &archive) {
    archive >> _actor >> _action;
    if (!dynamic_cast<SequenceItemLeader*>(this) && !dynamic_cast<SequenceItemDefaultAction*>(this))
        debug("\t\tSequenceItem: _actor = %s, _action = %s", _actor.c_str(), _action.c_str());
}

const Common::String &SequenceItem::getActor() const {
    return _actor;
}

const Common::String &SequenceItem::getAction() const {
    return _action;
}

} // End of namespace Pink