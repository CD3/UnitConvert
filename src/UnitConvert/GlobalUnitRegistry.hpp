#pragma once
#include "./UnitRegistry.hpp"

namespace UnitConvert {
/**
 * Return a reference to the global unit registry. This registry
 * can be accessed from multiple scopes so that you do not have
 * to create and build a unit registry in every place you need one.
 *
 * By default, a (large) set of units will be added to the registry.
 * This registry should be sufficient for most use cases without the
 * need to add any units.
 */
UnitRegistry& getGlobalUnitRegistry(bool loadUnits = true);
}


using UnitConvert::getGlobalUnitRegistry;
