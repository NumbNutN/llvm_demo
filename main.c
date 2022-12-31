#include <stdarg.h>  //变长参数函数所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Pass.h"
#include "bblock.h"
#include "c_container_auxiliary.h"
#include "cds.h"
#include "symbol_table.h"

extern List *ins_list;
extern List *func_list;

SymbolTable *cur_symboltable = NULL;

Value *return_val = NULL;

ast *pre_astnode = NULL;

BasicBlock *cur_bblock = NULL;

Function *cur_func = NULL;

int yyparse(void);
int parser(char *input);

int main() {
  AllInit();

  // if (freopen("out.txt", "w", stdout) == NULL) {
  //   fprintf(stderr, "打开文件失败！");
  //   exit(-1);
  // }

  printf("开始遍历\n");

  return_val = (Value *)malloc(sizeof(Value));
  value_init(return_val);
  return_val->name = strdup("return_val");
  return_val->VTy->TID = DefaultTyID;

  // if嵌套的例子
  char *input1 =
      "int main() {"
      "  int a;"
      "  int b;"
      "  int c;"
      "  a = 5;"
      "  b = 10;"
      "  if (a == 5) {"
      "    a = 30;"
      "    b = 40;"
      "    c = a + b;"
      "    if (b == 20) {"
      "      c = 19;"
      "      a = 233;"
      "    }"
      "    a = 156;"
      "  } else {"
      "    a = 100;"
      "    b = 200;"
      "    c = 120 + a;"
      "  }"
      "  int m = 30, n = 23;"
      "  return a;"
      "}";

  parser(input1);

  print_ins_pass(ins_list);
  printf("遍历结束\n\n");

  // 清除return语句和label或者functionEnd之间语句之间的不可达语句的pass
  delete_return_deadcode_pass(ins_list);

  if (freopen("out.txt", "w", stdout) == NULL) {
    fprintf(stderr, "打开文件失败！");
    exit(-1);
  }

  ins_toBBlock_pass(ins_list);

  print_bblock_pass(cur_func->entry_bblock);

  ListSetClean(ins_list, CleanObject);
  ListDeinit(ins_list);

  return 0;
}
