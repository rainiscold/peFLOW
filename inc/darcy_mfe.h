// ---------------------------------------------------------------------
//
// Copyright (C) 2016 - 2017 Ilona Ambartsumyan, Eldar Khattatov
//
// This file is part of peFLOW.
//
// ---------------------------------------------------------------------

#ifndef PEFLOW_DARCY_MFE_H
#define PEFLOW_DARCY_MFE_H

#include <deal.II/lac/block_sparse_matrix.h>
#include <deal.II/lac/block_vector.h>
#include <deal.II/grid/tria.h>

#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/base/parsed_function.h>

#include <deal.II/base/convergence_table.h>
#include <deal.II/base/timer.h>

#include "../inc/problem.h"
#include "../inc/utilities.h"
#include "../inc/darcy_data.h"


namespace darcy
{
  using namespace dealii;

  template <int dim>
  class MixedDarcyProblem : public Problem<dim>
  {
  public:
    MixedDarcyProblem(const unsigned int degree,
                      ParameterHandler &);
    void run(const unsigned int refine, const unsigned int grid);
  private:
    ParameterHandler &prm;

    struct CellAssemblyScratchData
    {
      CellAssemblyScratchData (const FiniteElement<dim> &fe,
                               const Quadrature<dim>    &quadrature,
                               const Quadrature<dim-1>  &face_quadrature,
                               const KInverse<dim> &k_data,
                               Functions::ParsedFunction<dim> *rhs,
                               Functions::ParsedFunction<dim> *bc);
      CellAssemblyScratchData (const CellAssemblyScratchData &scratch_data);
      FEValues<dim>     fe_values;
      FEFaceValues<dim> fe_face_values;
      KInverse<dim>     K_inv;
      Functions::ParsedFunction<dim> *bc;
      Functions::ParsedFunction<dim> *rhs;
    };

    struct CellAssemblyCopyData
    {
      FullMatrix<double>                   cell_matrix;
      Vector<double>                       cell_rhs;
      std::vector<types::global_dof_index> local_dof_indices;
    };

    void make_grid_and_dofs();
    void assemble_system();
    void assemble_system_cell (const typename DoFHandler<dim>::active_cell_iterator &cell,
                               CellAssemblyScratchData                             &scratch,
                               CellAssemblyCopyData                                &copy_data);
    void copy_local_to_global (const CellAssemblyCopyData &copy_data);

    void solve();
    void compute_errors (const unsigned int cycle);
    void output_results (const unsigned int cycle,  const unsigned int refine);

    const unsigned int degree;

    Triangulation<dim> triangulation;
    FESystem<dim>      fe;
    DoFHandler<dim>    dof_handler;


    BlockSparsityPattern      sparsity_pattern;
    BlockSparseMatrix<double> system_matrix;

    BlockVector<double> solution;
    BlockVector<double> system_rhs;

    ConvergenceTable convergence_table;
    TimerOutput      computing_timer;
  };
}

#endif //PEFLOW_DARCY_MFE_H
