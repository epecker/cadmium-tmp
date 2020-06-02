/**
 * Copyright (c) 2020, Román Cárdenas Rodríguez
 * ARSLab - Carleton University
 * GreenLSI - Polytechnic University of Madrid
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CADMIUM_CELLDEVS_GRID_COUPLED_HPP
#define CADMIUM_CELLDEVS_GRID_COUPLED_HPP

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/celldevs/coupled/cells_coupled.hpp>
#include <cadmium/celldevs/cell/grid_cell.hpp>
#include <cadmium/celldevs/utils/grid_utils.hpp>


namespace cadmium::celldevs {
    /**
     * Coupled Cell-DEVS model for Lattice-based scenarios
     * @tparam T the type used for representing time in a simulation.
     * @tparam S the type used for representing a cell state.
     * @tparam V the type used for representing a neighboring cell's vicinity. By default, it is set to integer.
     * @see coupled/cells_coupled.hpp
     */
    template <typename T, typename S, typename V=int>
    class grid_coupled: public cells_coupled<T, cell_position, S, V, seq_hash<cell_position>> {
    public:

        using cells_coupled<T, cell_position, S, V, seq_hash<cell_position>>::get_cell_name;
        using cells_coupled<T, cell_position, S, V, seq_hash<cell_position>>::add_cell_vicinity;

        /**
         * @brief constructor method
         * @param id ID of the Coupled DEVS model that contains the Cell-DEVS scenario
         */
        explicit grid_coupled(std::string const &id) : cells_coupled<T, cell_position, S, V, seq_hash<cell_position>>(id) {}

        /**
         * @brief adds a lattice of cells to the coupled model
         * @tparam CELL_MODEL model type of the cell to be included
         * @tparam Args any additional parameter required for initializing the cell model
         * @param scenario grid-based Cell-DEVS scenario containing all the information of the lattice
         * @param delayer_id ID identifying the type of output delayer buffer used by the cell.
         * @param args any additional parameter required for initializing the cell model
         */
        template <template <typename> typename CELL_MODEL, typename... Args>
        void add_lattice(grid_scenario<S, V> &scenario, std::string const &delayer_id, Args&&... args) {
            for (auto const &cell: scenario.get_states()) {
                cell_position cell_id = cell.first;
                cell_map<S, V> map = scenario.get_cell_map(cell_id);
                cadmium::dynamic::modeling::coupled<T>::_models.push_back(
                        cadmium::dynamic::translate::make_dynamic_atomic_model<CELL_MODEL, T>(get_cell_name(cell_id),
                                                                                              map, delayer_id,
                                                                                              std::forward<Args>(args)...));
            }
            for (auto const &cell: scenario.get_states()) {
                cell_position cell_to = cell.first;
                cell_unordered<V> vicinities = scenario.get_cell_map(cell_to).vicinity;
                add_cell_vicinity(cell_to, vicinities);
            }
        }
    };
} //namespace cadmium::celldevs
#endif //CADMIUM_CELLDEVS_GRID_COUPLED_HPP
