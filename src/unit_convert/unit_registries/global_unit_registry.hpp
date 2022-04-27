#pragma once
#include "./si_expression_unit_registry.hpp"


namespace unit_convert {

/**
 * Returns a reference to the global unit registry. This registry
 * can be accessed from multiple scopes so that you do not have
 * to create and build a unit registry in every place you need one.
 *
 * By default, a (large) set of units will be added to the registry.
 * This registry should be sufficient for most use cases without the
 * need to add any units. Special or custom units can be added,
 * and they will be available to all other users.
 */
  si_expression_unit_registry<double>& get_global_unit_registry(bool a_load_units = true);
}

