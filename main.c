#include <stdarg.h> //变长参数函数所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Ast.h"
#include "Pass.h"
#include "bblock.h"
#include "c_container_auxiliary.h"
#include "cds.h"
#include "symbol_table.h"

#include "config.h"
#include <sys/stat.h>

extern List *ins_list;
extern List *func_list;
extern List *global_var_list;

SymbolTable *cur_symboltable = NULL;

Value *return_val = NULL;

ast *pre_astnode = NULL;

int yyparse(void);

int parser(char *input);

char *tty_path;

char *read_code_from_file(const char *);

char *test_cases[] = {"./test_cases/00_main.c",
                      "./test_cases/01_var_defn2.c",
                      "./test_cases/02_var_defn3.c",
                      "./test_cases/03_arr_defn2.c",
                      "./test_cases/04_arr_defn3.c",
                      "./test_cases/05_arr_defn4.c",
                      "./test_cases/06_const_var_defn2.c",
                      "./test_cases/07_const_var_defn3.c",
                      "./test_cases/08_const_array_defn.c",
                      "./test_cases/09_func_defn.c",
                      "./test_cases/10_var_defn_func.c",
                      "./test_cases/11_add2.c",
                      "./test_cases/12_addc.c",
                      "./test_cases/13_sub2.c",
                      "./test_cases/14_subc.c",
                      "./test_cases/15_mul.c",
                      "./test_cases/16_mulc.c",
                      "./test_cases/17_div.c",
                      "./test_cases/18_divc.c",
                      "./test_cases/19_mod.c",
                      "./test_cases/20_rem.c",
                      "./test_cases/21_if_test2.c",
                      "./test_cases/22_if_test3.c",
                      "./test_cases/23_if_test4.c",
                      "./test_cases/24_if_test5.c",
                      "./test_cases/25_while_if.c",
                      "./test_cases/26_while_test1.c",
                      "./test_cases/27_while_test2.c",
                      "./test_cases/28_while_test3.c",
                      "./test_cases/29_break.c",
                      "./test_cases/30_continue.c",
                      "./test_cases/31_while_if_test1.c",
                      "./test_cases/32_while_if_test2.c",
                      "./test_cases/33_while_if_test3.c",
                      "./test_cases/34_arr_expr_len.c",
                      "./test_cases/35_op_priority1.c",
                      "./test_cases/36_op_priority2.c",
                      "./test_cases/37_op_priority3.c",
                      "./test_cases/38_op_priority4.c",
                      "./test_cases/39_op_priority5.c",
                      "./test_cases/40_unary_op.c",
                      "./test_cases/41_unary_op2.c",
                      "./test_cases/42_empty_stmt.c",
                      "./test_cases/43_logi_assign.c",
                      "./test_cases/44_stmt_expr.c",
                      "./test_cases/45_comment1.c",
                      "./test_cases/46_hex_defn.c",
                      "./test_cases/47_hex_oct_add.c",
                      "./test_cases/48_assign_complex_expr.c",
                      "./test_cases/49_if_complex_expr.c",
                      "./test_cases/50_short_circuit.c",
                      "./test_cases/51_short_circuit3.c",
                      "./test_cases/52_scope.c",
                      "./test_cases/53_scope2.c",
                      "./test_cases/54_hidden_var.c",
                      "./test_cases/55_sort_test1.c",
                      "./test_cases/56_sort_test2.c",
                      "./test_cases/57_sort_test3.c",
                      "./test_cases/58_sort_test4.c",
                      "./test_cases/59_sort_test5.c",
                      "./test_cases/60_sort_test6.c",
                      "./test_cases/61_sort_test7.c",
                      "./test_cases/62_percolation.c",
                      "./test_cases/63_big_int_mul.c",
                      "./test_cases/64_calculator.c",
                      "./test_cases/65_color.c",
                      "./test_cases/66_exgcd.c",
                      "./test_cases/67_reverse_output.c",
                      "./test_cases/68_brainfk.c",
                      "./test_cases/69_expr_eval.c",
                      "./test_cases/70_dijkstra.c",
                      "./test_cases/71_full_conn.c",
                      "./test_cases/72_hanoi.c",
                      "./test_cases/73_int_io.c",
                      "./test_cases/74_kmp.c",
                      "./test_cases/75_max_flow.c",
                      "./test_cases/76_n_queens.c",
                      "./test_cases/77_substr.c",
                      "./test_cases/78_side_effect.c",
                      "./test_cases/79_var_name.c",
                      "./test_cases/80_chaos_token.c",
                      "./test_cases/81_skip_spaces.c",
                      "./test_cases/82_long_func.c",
                      "./test_cases/83_long_array.c",
                      "./test_cases/84_long_array2.c",
                      "./test_cases/85_long_code.c",
                      "./test_cases/86_long_code2.c",
                      "./test_cases/87_many_params.c",
                      "./test_cases/88_many_params2.c",
                      "./test_cases/89_many_globals.c",
                      "./test_cases/90_many_locals.c",
                      "./test_cases/91_many_locals2.c",
                      "./test_cases/92_register_alloc.c",
                      "./test_cases/93_nested_calls.c",
                      "./test_cases/94_nested_loops.c",
                      "./test_cases/95_float.c",
                      "./test_cases/96_matrix_add.c",
                      "./test_cases/97_matrix_sub.c",
                      "./test_cases/98_matrix_mul.c",
                      "./test_cases/99_matrix_tran.c"};

int main(int argc, char **argv) {
  tty_path = ttyname(STDIN_FILENO);

  AllInit();


  printf("%%begin the pass\n");
  char *choose_case = NULL;
  if (argc == 5) {
    choose_case = read_code_from_file(argv[4]);
  } else {
    assert("invalid parameters");
  }
  if (choose_case == NULL)
    return 1;

  int saveSTDOUT =dup(STDOUT_FILENO);

#ifdef DEBUG_MODE
  freopen("./output/printf_ast.txt", "w", stdout);
#endif

#define PARSER
  parser(choose_case);

#ifdef PARSER

#ifdef DEBUG_MODE
  freopen(tty_path, "w", stdout);
  freopen("./output/out.txt", "w", stdout);
#endif

#ifdef DEBUG_MODE
  print_ins_pass(ins_list);
#endif

  delete_return_deadcode_pass(ins_list);

  ins_toBBlock_pass(ins_list);

  print_ins_pass(global_var_list);

  TranslateInit();
  //翻译全局变量表
  translate_global_variable_list(global_var_list);
  
  ListFirst(func_list, false);
  void *element;
  while (ListNext(func_list, &element)) {
    puts(((Function *)element)->label->name);
    bblock_to_dom_graph_pass((Function *)element);
  }
#endif

  /* 生成文件 */
  freopen(argv[3], "w", stdout);
  print_model();

  free(tty_path);
#ifdef DEBUG_MODE
  // dup2(saveSTDOUT,STDOUT_FILENO);
  // printf("All over!\n");
#endif
  return 0;
}

char *read_code_from_file(const char *file_path) {
  puts(file_path);
  FILE *fd = fopen(file_path, "r");

  if (fd == NULL) {
    perror("fopen()");
    return NULL;
  }

  fseek(fd, 0, SEEK_END);
  long file_size = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  char *buffer = (char *)malloc(file_size + 1);
  if (buffer == NULL) {
    printf("malloc() error\n");
    fclose(fd);
    return NULL;
  }
  size_t bytes_read = fread(buffer, 1, file_size, fd);
  buffer[bytes_read] = '\0';
  fclose(fd);
  return buffer;
}
