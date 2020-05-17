// Define A and rhs for
// -u'' = 1; u(0) = u(1) = 0
//  tridiag matrix A = [-1 2 -1] 
//  RHS^T = (1,1,1, ... ,1) * h**2
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <vector>
#include "dmumps_c.h"
#define JOB_INIT -1
#define JOB_END -2
#define USE_COMM_WORLD -987654

#if defined(MAIN_COMP)
/*
 * Some Fortran compilers (COMPAQ fort) define main inside
 * their runtime library while a Fortran program translates
 * to MAIN_ or MAIN__ which is then called from "main". This
 * is annoying because MAIN__ has no arguments and we must
 * define argc/argv arbitrarily !!
 */
int MAIN__();
int MAIN_()
  {
    return MAIN__();
  }

int MAIN__()
{
  int argc=1;
  char * name = "c_example";
  char ** argv ;
#else
int main(int argc, char ** argv)
{
#endif
  DMUMPS_STRUC_C id;


  MUMPS_INT n = 90001;
  MUMPS_INT8 nnz = 3 * n - 2;

  double dL = 1.0 / (n + 1);

  std::vector<MUMPS_INT> irn(nnz);
  std::vector<MUMPS_INT> jcn(nnz);
  std::vector<double> a(nnz);
  std::vector<double> rhs(n,dL*dL);
  int offset = 1;

  std::vector<double> U_exact(n,0);
  int cnt = 0;
  for (int row = 0 ; row < n ; row++){
    int col0 = row - 1 < 0 ? 0 : row -1;
    int col1 = row + 1 < n ? row + 1 + offset : row + offset;
    for (int  col = col0 ; col < col1 ; col++){
      if (row == col)
      {
        irn[cnt] = row + offset;
        jcn[cnt] = col + offset;
        a[cnt] = 2;
        cnt++;
      }
      else if (abs(row-col)==1)
      {
        irn[cnt] = row + offset;
        jcn[cnt] = col + offset;
        a[cnt] = -1;
        cnt++;
      }
    }
  }


  int error = 0;
#if defined(MAIN_COMP)
  argv = &name;
#endif

  id.par=1; id.sym=0;
  id.job=JOB_INIT;
  dmumps_c(&id);

  id.n = n;
  id.nnz = nnz; 
  id.irn=irn.data();
  id.jcn=jcn.data();
  id.a = a.data();
  id.rhs = rhs.data();
#define ICNTL(I) icntl[(I)-1] /* macro s.t. indices match documentation */
  /* No outputs */
  id.ICNTL(1)=-1; id.ICNTL(2)=-1; id.ICNTL(3)=-1; id.ICNTL(4)=0;

  /* Call the MUMPS package (analyse, factorization and solve). */
  id.job=6;
  dmumps_c(&id);
  if (id.infog[0]<0) {
    printf(" (PROC %d) ERROR RETURN: \tINFOG(1)= %d\n\t\t\t\tINFOG(2)= %d\n",
        0, id.infog[0], id.infog[1]);
    error = 1;
  }

  /* Terminate instance. */
  id.job=JOB_END;
  dmumps_c(&id);

  double xi(dL);
  double sum_del_U(0), norm_del_U(0);
  double sum_U(0), norm_U(0);

  if (!error) {
    for (int dof = 0; dof < n; dof++)
    {
      U_exact[dof] =  -0.5 * xi * (xi - 1.0);
      sum_del_U += pow(U_exact[dof] - rhs[dof],2);
      sum_U += pow(rhs[dof],2);
      xi += dL;
    }
    norm_del_U = sqrt(sum_del_U);
    norm_U = sqrt(sum_U);
    std::cout << "|| u_mumps - u_analytic || = " << norm_del_U / norm_U << '\n';
  } else {
    printf("An error has occured, please check error code returned by MUMPS.\n");
  }
  return 0;
}
