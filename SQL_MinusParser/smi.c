#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
typedef struct field {
  char *name;
  int type;
  bool is_nnull;
  bool is_prk;
  bool is_unq;
  int size_char;
  int val[128];
  char *str[128];
  bool isnull[128];
} FIELD;
typedef struct table {
  char *name;
  int num_field;
  FIELD field[256];
  int num_record;
  int serecord[128];
} TABLE;
char *key_word[24] = {"INT", "CHAR", "CREATE", "TABLE", "PRIMARY", "KEY", "NOT", "NULL", "UNIQUE", "INSERT", "INTO", "VALUES", "DELETE", "FROM", "WHERE", "UPDATE", "ORDER", "BY", "ASC", "DESC", "AND", "OR", "BETWEEN", "IS"};
int cline;
int line = 0;
char *sql_lan[2000] = {0};
char *lan_start[2000] = {0};
TABLE table[1000];
int sum_table;
int fail_flag;
int e;
// to initial
void reset_table(int num_table);
void reset_field(FIELD *n);
void clean_a_record(int num_table, int num_record);
// to check
bool is_kewrd(char *name);
bool is_reprk(TABLE n);
int is_exist_table(char *tar);
void print_table(int curtable, int curfield[], int sumfield);
// to run
void skip_space();
void skip_error_sen(int i);
// to parse
void read_sql();
void parse_sql();
void parse_create_table();
void parse_create_field_table();
void parse_insert_into();
void parse_condition_clause(int curtable, int numrecord, int serecord[]);
void parse_select();
void parse_where_and_order(int curtable, int curfield[]);
void parse_update();
void parse_delete_from();
void parse_conjunctable_clause(int curtable, int numrecord, int serecord[]);
void parse_negatable_clause(int curtable, int numrecord, int serecord[]);
void parse_automic_clause(int curtable, int numrecord, int serecord[]);
void parse_wrong_condition_clause();
void parse_wrong_where_and_order();
// to help parse
void get_data(int curtable, int val[], char *str[], int curfield[], int *sum, int t[]);
int check_cmpo();
int and_or(int type, int val, int cur, int not );
int cmporesint(int cmpo, int v1, int v2);
int cmporesstr(int cmpo, char *s1, char *s2);
void sort_prk(int curtable);
void sort_field(int curtable, int rank_field[], int rank_type[], int sum_rank_field);
void swap_two_records(int curtable, int r1, int r2);
int main() {
  freopen("test.txt", "r", stdin);
  read_sql();
  parse_sql();
}
void reset_table(int num_table) {
  for (int i = 0; i <= table[num_table].num_field; i++) {
    reset_field(&table[num_table].field[i]);
  }
  if (table[num_table].name != NULL) {
    free(table[num_table].name);
  }
  table[num_table].name = NULL;
  table[num_table].num_field = 0;
  table[num_table].num_record = 0;
}
void reset_field(FIELD *n) {
  if ((*n).name != NULL) {
    free((*n).name);
  }
  (*n).name = NULL;
  (*n).type = 0;  // 1Ϊint��2Ϊchar[]
  (*n).is_nnull = 0;
  (*n).is_prk = 0;
  (*n).is_unq = 0;
  (*n).size_char = 0;  // ��Ϊ�ַ��Ŀռ�
}
void clean_a_record(int num_table, int num_record) {
  for (int i = 0; i < table[num_table].num_field; i++) {
    if (table[num_table].field[i].str[num_record] != NULL)
      free(table[num_table].field[i].str[num_record]);
    table[num_table].field[i].val[num_record] = 0;
    table[num_table].field[i].str[num_record] = NULL;
    table[num_table].field[i].isnull[num_record] = 1;
  }
}
bool is_kewrd(char *name) {
  for (int i = 0; i < 24; i++) {
    if (!strcmp(name, key_word[i])) {
      return true;
    }
  }
  return false;
}
bool is_reprk(TABLE n) {
  int sum = 0;
  for (int i = 0; i <= n.num_field; i++) {
    if (n.field[i].is_prk) {
      sum++;
    }
  }
  return sum == 1 ? false : true;
}
int is_exist_table(char *tar) {
  for (int i = 0; i < sum_table; i++) {
    if (!strcmp(table[i].name, tar)) {
      return i;
    }
  }
  return -1;
}
int is_exist_table_field(int curtable, char *tar) {
  if (curtable == -1) {
    return -1;
  }
  for (int i = 0; i < table[curtable].num_field; i++) {
    if (!strcmp(tar, table[curtable].field[i].name)) {
      return i;
    }
  }
  return -1;
}
void skip_space() {
  while (sql_lan[cline][0] == ' ' || sql_lan[cline][0] == '\0') {
    if (sql_lan[cline][0] == '\0') {
      free(lan_start[cline]);
      cline++;
    } else {
      sql_lan[cline]++;
    }
  }
}
void skip_error_sen(int i) {
  if (i == 1) {
    printf("SYNTAX ERROR\n");
  } else if (i == 2) {
    printf("ERROR\n");
  }
  while (cline < line && (sql_lan[cline][0] == '\0' || sql_lan[cline][strlen(sql_lan[cline]) - 1] != ';')) {
    if (lan_start[cline] != NULL) {
      free(lan_start[cline]);
    }
    cline++;
  }
}
void read_sql() {
  char read_line[2049] = {0};  // ��ʱ��ȡһ�仰
  while (fgets(read_line, 2049, stdin)) {
    int bi = 0;
    read_line[strlen(read_line)] = 0;
    while (read_line[bi] == ' ') {
      bi++;
    }
    sql_lan[line] = (char *)malloc(strlen(read_line) + 1 - bi);
    strcpy(sql_lan[line], read_line + bi);
    lan_start[line] = sql_lan[line];
    line++;
  }
  for (int i = 0; i < line - 1; i++) {
    sql_lan[i][strlen(sql_lan[i]) - 1] = 0;
  }
  int t = line - 1;
  while (sql_lan[t][0] == '\n' || sql_lan[t][0] == 0) {
    sql_lan[t][0] = 0;
    t--;
  }
  sql_lan[t][strlen(sql_lan[t])] = ';';

  // if (sql_lan[line - 1][0] != 0) {
  //   sql_lan[line - 1][strlen(sql_lan[line - 1])] = ';';
  //   sql_lan[line - 1][strlen(sql_lan[line - 1]) + 1] = 0;
  // } else {
  //   line--;
  // }
}
void parse_sql() {
  for (cline = 0; cline < line; cline++) {
    if (!strncmp(sql_lan[cline], "CREATE", 6)) {
      sql_lan[cline] += 6;
      skip_space();
      if (!strncmp(sql_lan[cline], "TABLE", 5)) {
        sql_lan[cline] += 5;
        skip_space();
        parse_create_table();
      } else {
        skip_error_sen(1);
      }
    } else if (!strncmp(sql_lan[cline], "INSERT", 6)) {
      sql_lan[cline] += 6;
      skip_space();
      if (!strncmp(sql_lan[cline], "INTO", 4)) {
        sql_lan[cline] += 4;
        skip_space();
        parse_insert_into();
      } else {
        skip_error_sen(1);
      }
    } else if (!strncmp(sql_lan[cline], "DELETE", 6)) {
      sql_lan[cline] += 6;
      skip_space();
      if (!strncmp(sql_lan[cline], "FROM", 4)) {
        sql_lan[cline] += 4;
        skip_space();
        parse_delete_from();
      } else {
        skip_error_sen(1);
      }
    } else if (!strncmp(sql_lan[cline], "UPDATE", 6)) {
      sql_lan[cline] += 6;
      skip_space();
      parse_update();
    } else if (!strncmp(sql_lan[cline], "SELECT", 6)) {
      sql_lan[cline] += 6;
      skip_space();
      parse_select();
    } else if (sql_lan[cline][0] == '\0') {
    } else {
      skip_error_sen(1);
    }
    if (lan_start[cline] != NULL) {
      free(lan_start[cline]);
    }
  }
}
void parse_create_table() {
  // ��ȡ����
  char *name = malloc(33);
  int len = 0;
  while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
    name[len++] = *sql_lan[cline]++;
  }
  name[len] = '\0';
  if (len == 0) {
    skip_error_sen(1);
    return;
  }
  skip_space();
  table[sum_table].name = name;
  // ͣȡ����
  if (sql_lan[cline][0] == '(') {
    sql_lan[cline]++;
    skip_space();
  } else {
    skip_error_sen(1);
    return;
  }
  // ��ȡ�ֶ�
  if (sql_lan[cline][0] == ')') {
    skip_error_sen(1);
    return;
  }
  while (sql_lan[cline][0] != ')') {
    parse_create_field_table();
    if (fail_flag) {
      fail_flag = 0;
      reset_table(sum_table);
      return;
    }
  }
  if (fail_flag) {
    fail_flag = 0;
    return;
  }
  // �����������ֶ����Ƿ���ؼ��ֳ�ͻ
  if (is_kewrd(table[sum_table].name)) {
    reset_table(sum_table);
    skip_error_sen(1);
    return;
  } else {
    for (int i = 0; i < table[sum_table].num_field; i++) {
      if (is_kewrd(table[sum_table].field[i].name)) {
        reset_table(sum_table);
        skip_error_sen(1);
        return;
      }
    }
  }
  // ���������Ƿ�ֻ��һ������
  if (is_reprk(table[sum_table])) {
    reset_table(sum_table);
    skip_error_sen(2);
    return;
  }
  // ����Ƿ����ظ��ֶ���
  for (int i = 0; i < table[sum_table].num_field - 1; i++) {
    for (int j = i + 1; j < table[sum_table].num_field; j++) {
      if (!strcmp(table[sum_table].field[i].name, table[sum_table].field[j].name)) {
        skip_error_sen(2);
        reset_table(sum_table);
        return;
      }
    }
  }
  // �������Ƿ��ظ�
  if (is_exist_table(name) != -1) {
    reset_table(sum_table);
    skip_error_sen(2);
    return;
  }
  printf("CREATE TABLE SUCCESSFULLY\n");
  sum_table++;
}
void parse_create_field_table() {
  // ��ȡ�ֶ���
  char *name = malloc(33);
  int len = 0;
  while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
    name[len++] = *sql_lan[cline]++;
  }
  if (len == 0) {
    fail_flag = 1;
    skip_error_sen(1);
    return;
  }
  name[len] = '\0';
  table[sum_table].field[table[sum_table].num_field].name = name;
  skip_space();
  // ȡ�ֶ�������
  if (!strncmp(sql_lan[cline], "INT", 3)) {
    sql_lan[cline] += 3;
    table[sum_table].field[table[sum_table].num_field].type = 1;
  } else if (!strncmp(sql_lan[cline], "CHAR", 4)) {
    sql_lan[cline] += 4;
    table[sum_table].field[table[sum_table].num_field].type = 2;
    skip_space();
    if (sql_lan[cline][0] == '(') {
      sql_lan[cline]++;
      int sum = atoi(sql_lan[cline]);
      if (sum <= 0) {
        fail_flag = 1;
        skip_error_sen(2);
        return;
      }
      skip_space();
      if (!isdigit(sql_lan[cline][0])) {
        fail_flag = 1;
        skip_error_sen(1);
        return;
      }
      table[sum_table].field[table[sum_table].num_field].size_char = sum;
      while (isdigit(sql_lan[cline][0])) {
        sql_lan[cline]++;
      }
      skip_space();
      if (sql_lan[cline][0] == ')') {
        sql_lan[cline]++;
      } else {
        fail_flag = 1;
        skip_error_sen(1);
        return;
      }
    } else {
      fail_flag = 1;
      skip_error_sen(1);
      return;
    }
  } else {
    skip_error_sen(1);
    fail_flag = 1;
    return;
  }
  // printf("%s\n", sql_lan[cline]);
  // if (sql_lan[cline][0] == '\0') {
  //   table[sum_table].num_field++;
  //   skip_space();
  //   return;
  // }
  skip_space();
  if (!strncmp(sql_lan[cline], "NOT", 3)) {
    sql_lan[cline] += 3;
    skip_space();
    if (!strncmp(sql_lan[cline], "NULL", 4)) {
      sql_lan[cline] += 4;
      table[sum_table].field[table[sum_table].num_field].is_nnull = 1;
      skip_space();
    } else {
      fail_flag = 1;
      skip_error_sen(1);
      return;
    }
  }
  if (!strncmp(sql_lan[cline], "PRIMARY", 7)) {
    sql_lan[cline] += 7;
    skip_space();
    if (!strncmp(sql_lan[cline], "KEY", 3)) {
      sql_lan[cline] += 3;
      table[sum_table].field[table[sum_table].num_field].is_prk = 1;
      table[sum_table].field[table[sum_table].num_field].is_nnull = 1;
      table[sum_table].field[table[sum_table].num_field].is_unq = 1;
      skip_space();
    }
  }
  if (!strncmp(sql_lan[cline], "UNIQUE", 6)) {
    sql_lan[cline] += 6;
    table[sum_table].field[table[sum_table].num_field].is_unq = 1;
    skip_space();
  }
  if (sql_lan[cline][0] == ',') {
    table[sum_table].num_field++;
    sql_lan[cline]++;
    skip_space();
    if (sql_lan[cline][0] == ')') {
      skip_error_sen(1);
      fail_flag = 1;
      return;
    }
  } else if (sql_lan[cline][0] == ')') {
    table[sum_table].num_field++;
  } else {
    fail_flag = 1;
    skip_error_sen(1);
    return;
  }
}
void parse_insert_into() {
  int error = 0;
  // ȡ������ʼ
  char *tartable = sql_lan[cline];
  while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
    sql_lan[cline]++;
  }
  if (sql_lan[cline][0] == ' ') {
    sql_lan[cline][0] = 0;
    sql_lan[cline]++;
    skip_space();
  } else if (sql_lan[cline][0] == '\0') {
    skip_space();
  } else {
    skip_error_sen(1);
    return;
  }
  int curtable = is_exist_table(tartable);
  // ȡ��������
  if (curtable == -1) {
    error = 1;
  }
  if (!strncmp(sql_lan[cline], "VALUES", 6)) {
    sql_lan[cline] += 6;
    skip_space();
  } else {
    skip_error_sen(1);
    return;
  }
  if (sql_lan[cline][0] == '(') {
    sql_lan[cline]++;
    skip_space();
  } else {
    skip_error_sen(1);
    return;
  }
  int sumin = 0;
  int inval[256] = {0};
  char *instr[256] = {0};
  bool innull[256] = {0};
  while (sql_lan[cline][0] != ')') {
    if (sql_lan[cline][0] == ',') {
      skip_error_sen(1);
      return;
    }
    if (curtable >= 0 && sumin >= table[curtable].num_field) {
      error = 1;
      break;
    }
    if (!strncmp(sql_lan[cline], "NULL", 4)) {
      innull[sumin] = 1;
      sql_lan[cline] += 4;
    } else if (sql_lan[cline][0] == '\'') {
      instr[sumin] = malloc(1024);
      sql_lan[cline]++;
      int len = 0;
      while (sql_lan[cline][0] != '\'') {
        if (len >= 300) {
          break;
        }
        if (sql_lan[cline][0] == '\\' && sql_lan[cline][1] == '\'') {
          instr[sumin][len++] = '\\';
          instr[sumin][len++] = '\'';
          sql_lan[cline] += 2;
        } else {
          instr[sumin][len++] = *sql_lan[cline]++;
        }
      }
      sql_lan[cline]++;
      instr[sumin][len] = 0;
    } else {
      int val = atoi(sql_lan[cline]);
      skip_space();
      if (sql_lan[cline][0] == '-') {
        sql_lan[cline]++;
      }
      while (isdigit(sql_lan[cline][0])) {
        sql_lan[cline]++;
      }
      inval[sumin] = val;
    }
    skip_space();
    if (sql_lan[cline][0] == ',') {
      sumin++;
      sql_lan[cline]++;
      skip_space();
    } else if (sql_lan[cline][0] == ')') {
      sumin++;
      sql_lan[cline]++;
      break;
    } else {
      skip_error_sen(1);
      return;
    }
    if (sql_lan[cline][0] == ';') {
      skip_error_sen(1);
      return;
    }
  }
  //  ����ֶ����ͱ���
  if (error || sumin != table[curtable].num_field) {
    skip_error_sen(2);
    return;
  }
  for (int i = 0; i < table[curtable].num_field; i++) {
    if (table[curtable].field[i].type == 1) {
      if (innull[i] == 1) {
        table[curtable].field[i].isnull[table[curtable].num_record] = innull[i];
      } else if (instr[i] == NULL) {
        table[curtable].field[i].val[table[curtable].num_record] = inval[i];
        table[curtable].field[i].isnull[table[curtable].num_record] = 0;
      } else {
        clean_a_record(curtable, table[curtable].num_record);
        skip_error_sen(2);
        return;
      }
      // ���ǿգ�Ψһ
      if (table[curtable].field[i].is_nnull == 1 && innull[i]) {
        error = 1;
        break;
      }
      if (table[curtable].field[i].is_unq == 1) {
        for (int j = 0; j < table[curtable].num_record; j++) {
          if (innull[i] == 1) {
            break;
          }
          if (table[curtable].field[i].val[j] == inval[i]) {
            error = 1;
            break;
          }
        }
      }
    } else if (table[curtable].field[i].type == 2) {
      if (innull[i]) {
        table[curtable].field[i].isnull[table[curtable].num_record] = innull[i];
      } else if (instr[i] != NULL) {
        if (strlen(instr[i]) > table[curtable].field[i].size_char) {
          skip_error_sen(2);
          clean_a_record(curtable, table[curtable].num_record);
          return;
        }
        table[curtable].field[i].isnull[table[curtable].num_record] = 0;
        table[curtable].field[i].str[table[curtable].num_record] = malloc(table[curtable].field[i].size_char + 1);
        strcpy(table[curtable].field[i].str[table[curtable].num_record], instr[i]);
      } else {
        clean_a_record(curtable, table[curtable].num_record);
        skip_error_sen(2);
        return;
      }
      // ���ǿգ�Ψһ
      if (table[curtable].field[i].is_nnull == 1 && innull[i]) {
        error = 1;
        break;
      }
      if (table[curtable].field[i].is_unq == 1) {
        for (int j = 0; j < table[curtable].num_record; j++) {
          if (innull[i] == 1) {
            break;
          }
          if (table[curtable].field[i].isnull[j] == 0 && !strcmp(table[curtable].field[i].str[j], instr[i])) {
            error = 1;
            break;
          }
        }
      }
    }
    if (instr[i] != NULL)
      free(instr[i]);
  }
  if (error) {
    clean_a_record(curtable, table[curtable].num_record);
    skip_error_sen(2);
    return;
  }
  printf("1 RECORDS INSERTED\n");
  table[curtable].num_record++;
}
void parse_select() {
  int error = 0;
  if (sql_lan[cline][0] == '*') {
    // ��ѯ���м�¼
    sql_lan[cline]++;
    skip_space();
    if (!strncmp(sql_lan[cline], "FROM", 4)) {
      sql_lan[cline] += 4;
      skip_space();
    } else {
      skip_error_sen(1);
      return;
    }
    char *tartable = sql_lan[cline];
    while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
      sql_lan[cline]++;
    }
    if (sql_lan[cline][0] == ' ') {
      sql_lan[cline][0] = 0;
      sql_lan[cline]++;
      skip_space();
    } else if (sql_lan[cline][0] == ';') {
      sql_lan[cline][0] = 0;
      int curtable = is_exist_table(tartable);
      if (curtable < 0) {
        skip_error_sen(2);
        return;
      }
      // ��ӡ��
      for (int i = 0; i < table[curtable].num_record; i++) {
        table[curtable].serecord[i] = 1;
      }
      sort_prk(curtable);
      print_table(curtable, NULL, 0);
      return;
    } else {
      skip_error_sen(1);
      return;
    }
    int curtable = is_exist_table(tartable);
    if (curtable < 0) {
      error = 1;
    }
    fail_flag = 0;
    int serecord[128] = {0};
    e = 0;
    if (error == 0)
      parse_where_and_order(curtable, serecord);
    else
      parse_wrong_where_and_order();
    if (fail_flag) {
      skip_error_sen(1);
      fail_flag = 0;
      return;
    }
    if (e) {
      skip_error_sen(2);
      e = 0;
      return;
    }
    if (error) {
      skip_error_sen(2);
      return;
    }
    print_table(curtable, NULL, 0);
  } else {
    // ��ѯָ���ֶ�
    char *tarfield[10];
    int sumtar = 0;
    int curfield[10] = {0};
    while (strncmp(sql_lan[cline], "FROM", 4) != 0) {
      // ��ȡ�ֶ���
      tarfield[sumtar] = malloc(33);
      int len = 0;
      while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
        tarfield[sumtar][len++] = *sql_lan[cline]++;
      }
      if (len == 0) {
        skip_error_sen(1);
        return;
      }
      tarfield[sumtar][len] = '\0';
      // ȡ�ֶ�������
      if (sql_lan[cline][0] == ';') {
        //
        for (int i = 0; i <= sumtar; i++) {
          free(tarfield[i]);
        }
        skip_error_sen(1);
        return;
      }
      skip_space();
      if (sql_lan[cline][0] == ',') {
        sumtar++;
        sql_lan[cline]++;
        skip_space();
        if (!strncmp(sql_lan[cline], "FROM", 4)) {
          skip_error_sen(1);
          return;
        }
      }
      skip_space();
    }
    sumtar++;
    sql_lan[cline] += 4;
    skip_space();
    // ��ȡ����
    char *tartable = sql_lan[cline];
    while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
      sql_lan[cline]++;
    }
    if (sql_lan[cline][0] == ' ') {
      sql_lan[cline][0] = 0;
      sql_lan[cline]++;
      skip_space();
    } else if (sql_lan[cline][0] == ';') {
      sql_lan[cline][0] = 0;
      int curtable = is_exist_table(tartable);
      // ȡ��������
      for (int i = 0; i < sumtar; i++) {
        curfield[i] = is_exist_table_field(curtable, tarfield[i]);
      }
      // ȡ����ֶν���
      for (int i = 0; i < table[curtable].num_record; i++) {
        table[curtable].serecord[i] = 1;
      }
      sort_prk(curtable);
      print_table(curtable, curfield, sumtar);
      return;
      for (int i = 0; i < table[curtable].num_record; i++) {
        table[curtable].serecord[i] = 0;
      }
    }
    skip_space();
    int curtable = is_exist_table(tartable);
    for (int i = 0; i < sumtar; i++) {
      curfield[i] = is_exist_table_field(curtable, tarfield[i]);
    }
    // ȡ����ֶν���
    fail_flag = 0;
    int serecord[128] = {0};
    e = 0;
    if (error == 0)
      parse_where_and_order(curtable, serecord);
    else
      parse_wrong_where_and_order();
    if (fail_flag) {
      skip_error_sen(1);
      fail_flag = 0;
      return;
    }
    if (e) {
      e = 0;
      skip_error_sen(2);
      return;
    }
    if (error) {
      skip_error_sen(2);
      return;
    }
    print_table(curtable, curfield, sumtar);
  }
}
void parse_update() {
  // ��ȡ����
  int error = 0;
  char *tartable = sql_lan[cline];
  while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
    sql_lan[cline]++;
  }
  if (sql_lan[cline][0] == ' ') {
    sql_lan[cline][0] = 0;
    sql_lan[cline]++;
    skip_space();
  }
  int curtable = is_exist_table(tartable);
  // ȡ��������
  if (curtable == -1) {
    error = 1;  // ����������
  }
  if (!strncmp(sql_lan[cline], "SET", 3)) {
    sql_lan[cline] += 3;
    skip_space();
  } else {
    skip_error_sen(1);
    return;
  }
  int upfield[1000] = {0};
  int upval[1000] = {0};
  char *upstr[1000] = {0};
  bool upnull[1000] = {0};
  int sumup = 0;
  while (strncmp(sql_lan[cline], "WHERE", 5) != 0) {
    // ��ȡ�ֶ���
    char *tarfield = malloc(33);
    int len = 0;
    while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
      tarfield[len++] = *sql_lan[cline]++;
    }
    if (len == 0) {
      skip_error_sen(1);
      return;
    }
    tarfield[len] = 0;
    // ȡ�ֶ�������
    if (error == 0)
      upfield[sumup] = is_exist_table_field(curtable, tarfield);
    if (tarfield != NULL)
      free(tarfield);
    skip_space();
    if (upfield[sumup] == -1) {
      // �ֶβ�����
      error = 1;
    }
    if (sql_lan[cline][0] == '=') {
      sql_lan[cline]++;
      skip_space();
    } else {
      skip_error_sen(1);
      return;
    }
    if (error == 1) {
      if (!strncmp(sql_lan[cline], "NULL", 4)) {
        sql_lan[cline] += 4;
      } else if (sql_lan[cline][0] == '-' || isdigit(sql_lan[cline][0])) {
        sql_lan[cline]++;
        while (isdigit(sql_lan[cline][0])) {
          sql_lan[cline]++;
        }
      } else if (sql_lan[cline][0] == '\'') {
        sql_lan[cline]++;
        while (sql_lan[cline][0] != '\'') {
          if (sql_lan[cline][0] == '\\' && sql_lan[cline][1] == '\'') {
            sql_lan[cline] += 2;
          } else {
            sql_lan[cline]++;
          }
        }
        sql_lan[cline]++;
      } else {
        skip_error_sen(1);
        return;
      }
    } else if (table[curtable].field[upfield[sumup]].type == 1) {
      if (!strncmp(sql_lan[cline], "NULL", 4)) {
        sql_lan[cline] += 4;
        upnull[sumup] = 1;
      } else if (sql_lan[cline][0] == '-' || isdigit(sql_lan[cline][0])) {
        upval[sumup] = atoi(sql_lan[cline]);
        if (sql_lan[cline][0] == '-') {
          sql_lan[cline]++;
        }
        while (isdigit(sql_lan[cline][0])) {
          sql_lan[cline]++;
        }
      } else {
        error = 1;
      }
    } else if (table[curtable].field[upfield[sumup]].type == 2) {
      if (!strncmp(sql_lan[cline], "NULL", 4)) {
        upnull[sumup] = 1;
        sql_lan[cline] += 4;
      } else if (sql_lan[cline][0] == '\'') {
        char *tem = malloc(table[curtable].field[upfield[sumup]].size_char + 1);
        int len = 0;
        sql_lan[cline]++;
        while (sql_lan[cline][0] != '\'') {
          if (sql_lan[cline][0] == '\\' && sql_lan[cline][1] == '\'') {
            tem[len++] = sql_lan[cline][0];
            tem[len++] = sql_lan[cline][1];
            sql_lan[cline] += 2;
          } else {
            tem[len++] = *sql_lan[cline]++;
          }
          if (len > table[curtable].field[upfield[sumup]].size_char) {
            // error
            error = 1;
            while (sql_lan[cline][0] != '\'') {
              if (sql_lan[cline][0] == '\\' && sql_lan[cline][1] == '\'') {
                sql_lan[cline] += 2;
              } else {
                sql_lan[cline]++;
              }
            }
            sql_lan[cline]++;
            break;
          }
        }
        tem[len] = 0;
        sql_lan[cline]++;
        upstr[sumup] = tem;
      }
    }
    skip_space();
    if (sql_lan[cline][0] == ',') {
      sumup++;
      sql_lan[cline]++;
      continue;
    } else if (!strncmp(sql_lan[cline], "WHERE", 5)) {
      sumup++;
      break;
    } else {
      skip_error_sen(1);
      return;
    }
  }
  sql_lan[cline] += 5;
  skip_space();
  // ���������Ӿ�
  int serecord[128] = {0};
  fail_flag = 0;
  e = 0;
  if (error == 0)
    parse_condition_clause(curtable, table[curtable].num_record, serecord);
  else
    parse_wrong_condition_clause();
  skip_space();
  if (fail_flag) {
    skip_error_sen(1);
    fail_flag = 0;
    return;
  }
  if (e) {
    e = 0;
    skip_error_sen(2);
    return;
  }
  if (error) {
    skip_error_sen(2);
    return;
  }
  // ����Ƿ�UNQ
  for (int i = 0; i < sumup; i++) {
    if (table[curtable].field[upfield[i]].is_nnull == 1 && upnull[i]) {
      skip_error_sen(2);
      for (int i = 0; i < sumup; i++) {
        if (upstr[i] != NULL) {
          free(upstr[i]);
        }
      }
      return;
    }
    if (table[curtable].field[upfield[i]].is_unq == 1 && upnull[i] == 0) {
      int count = 0;
      for (int j = 0; j < table[curtable].num_record; j++) {
        if (serecord[j] == 1) {
          count++;
        } else if (table[curtable].field[upfield[i]].type == 1 && table[curtable].field[upfield[i]].val[j] == upval[i]) {
          count++;
        } else if (upstr[i] != NULL && table[curtable].field[upfield[i]].type == 2 && !strcmp(table[curtable].field[upfield[i]].str[j], upstr[i])) {
          count++;
        }
      }
      if (count > 1) {
        skip_error_sen(2);
        return;
      }
    }
  }
  for (int i = 0; i < table[curtable].num_record; i++) {
    // printf("%d\n", serecord[i]);
    table[curtable].serecord[i] = serecord[i];
  }
  // ��ʼ�޸�
  int count = 0;
  for (int i = 0; i < table[curtable].num_record; i++) {
    if (table[curtable].serecord[i] == 1) {
      table[curtable].serecord[i] = 0;
      count++;
      for (int j = 0; j < sumup; j++) {
        if (table[curtable].field[upfield[j]].type == 1) {
          if (upnull[j]) {
            table[curtable].field[upfield[j]].isnull[i] = upnull[j];
          } else {
            table[curtable].field[upfield[j]].val[i] = upval[j];
            table[curtable].field[upfield[j]].isnull[i] = 0;
          }
        } else if (table[curtable].field[upfield[j]].type == 2) {
          if (upnull[j]) {
            table[curtable].field[upfield[j]].isnull[i] = upnull[j];
            if (table[curtable].field[upfield[j]].str[i] != NULL) {
              free(table[curtable].field[upfield[j]].str[i]);
            }
          } else {
            table[curtable].field[upfield[j]].str[i] = malloc(table[curtable].field[upfield[j]].size_char + 1);
            strncpy(table[curtable].field[upfield[j]].str[i], upstr[j], strlen(upstr[j]));
            table[curtable].field[upfield[j]].str[i][strlen(upstr[j])] = 0;
            table[curtable].field[upfield[j]].isnull[i] = 0;
          }
        }
      }
    }
  }
  for (int j = 0; j < sumup; j++) {
    if (upstr[j] != NULL)
      free(upstr[j]);
  }
  printf("%d RECORDS UPDATED\n", count);
}
void parse_delete_from() {
  int error = 0;
  // ȡ������ʼ
  char *tartable = sql_lan[cline];
  while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
    sql_lan[cline]++;
  }
  if (sql_lan[cline][0] == ' ') {
    sql_lan[cline][0] = 0;
    sql_lan[cline]++;
    skip_space();
  } else if (sql_lan[cline][0] == '\0') {
    skip_space();
  } else {
    skip_error_sen(1);
    return;
  }
  int curtable = is_exist_table(tartable);
  // ȡ��������
  if (curtable == -1) {
    error = 1;
  }
  //
  if (!strncmp(sql_lan[cline], "WHERE", 5)) {
    sql_lan[cline] += 5;
    skip_space();
  } else {
    skip_error_sen(1);
    return;
  }
  if (sql_lan[cline][0] == ';') {
    skip_error_sen(1);
    return;
  }
  // ������򵥵������Ӿ�
  int serecord[128] = {0};
  fail_flag = 0;
  e = 0;
  if (curtable >= 0) {
    parse_condition_clause(curtable, table[curtable].num_record, serecord);
  } else {
    parse_wrong_condition_clause();
  }
  if (fail_flag == 1) {
    skip_error_sen(1);
    fail_flag = 0;
    return;
  }
  if (e) {
    e = 0;
    skip_error_sen(2);
    return;
  }
  if (error) {
    skip_error_sen(2);
    return;
  }
  skip_space();
  // ��ʼ�޸�
  for (int i = 0; i < table[curtable].num_record; i++) {
    table[curtable].serecord[i] = serecord[i];
  }
  int count = 0;
  for (int i = 0; i < table[curtable].num_record; i++) {
    if (table[curtable].serecord[i] == 1) {
      table[curtable].serecord[i] = 0;
      clean_a_record(curtable, i);
      count++;
    }
  }
  sort_prk(curtable);
  printf("%d RECORDS DELETED\n", count);
  table[curtable].num_record -= count;
}
void parse_where_and_order(int curtable, int serecord[]) {
  if (!strncmp(sql_lan[cline], "WHERE", 5)) {
    sql_lan[cline] += 5;
    skip_space();
    if (sql_lan[cline][0] == ';') {
      fail_flag = 1;
      return;
    }
    fail_flag = 0;
    parse_condition_clause(curtable, table[curtable].num_record, serecord);
    if (fail_flag) {
      return;
    }
    for (int i = 0; i < table[curtable].num_record; i++) {
      table[curtable].serecord[i] = serecord[i];
    }
    if (sql_lan[cline][0] == ';') {
      sort_prk(curtable);
      return;
    }
  } else if (curtable >= 0) {
    for (int i = 0; i < table[curtable].num_record; i++) {
      table[curtable].serecord[i] = 1;
    }
  }
  if (!strncmp(sql_lan[cline], "ORDER", 5)) {
    sql_lan[cline] += 5;
    skip_space();
    if (!strncmp(sql_lan[cline], "BY", 2)) {
      sql_lan[cline] += 2;
      skip_space();
      //
      int rank_type[100] = {0};
      int curfield[10] = {0};
      int sum_rank_field = 0;
      int rank_field[100] = {0};
      while (sql_lan[cline][0] != ';') {
        // ��ȡ�ֶ���
        char *tarfield = malloc(33);
        int len = 0;
        while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
          tarfield[len++] = *sql_lan[cline]++;
        }
        if (len == 0) {
          fail_flag = 1;
          return;
        }
        tarfield[len] = '\0';
        // ȡ�ֶ�������
        rank_field[sum_rank_field] = is_exist_table_field(curtable, tarfield);
        if (tarfield != NULL)
          free(tarfield);
        skip_space();
        if (!strncmp(sql_lan[cline], "ASC", 3)) {
          sql_lan[cline] += 3;
          skip_space();
        } else if (!strncmp(sql_lan[cline], "DESC", 4)) {
          sql_lan[cline] += 4;
          rank_type[sum_rank_field] = 1;
          skip_space();
        }
        if (sql_lan[cline][0] == ';') {
          sum_rank_field++;
          break;
        } else if (sql_lan[cline][0] == ',') {
          sql_lan[cline]++;
          skip_space();
          sum_rank_field++;
          continue;
        }
      }
      // ����
      sort_field(curtable, rank_field, rank_type, sum_rank_field);
      // for (int i = 0; i < table[curtable].num_record; i++) {
      //   printf("%s\n", table[curtable].field[rank_field[0]].str[i]);
      // }
    } else {
      fail_flag = 1;
      return;
    }
  } else {
    skip_space();
    if (sql_lan[cline][0] != ';') {
      fail_flag = 1;
      return;
    }
    sort_prk(curtable);
  }
}
void parse_condition_clause(int curtable, int numrecord, int serecord[]) {
  int type = 1;
  int tmp1[128] = {0};
  parse_conjunctable_clause(curtable, table[curtable].num_record, tmp1);
  skip_space();
  if (!strncmp(sql_lan[cline], "ORDER", 5)) {
    for (int i = 0; i < numrecord; i++) {
      serecord[i] = tmp1[i];
    }
    return;
  }
  if (!strncmp(sql_lan[cline], "OR", 2)) {
    sql_lan[cline] += 2;
    type = 3;
  } else {
    for (int i = 0; i < numrecord; i++) {
      serecord[i] = tmp1[i];
    }
    return;
  }
  skip_space();
  int tmp2[128] = {0};
  parse_condition_clause(curtable, table[curtable].num_record, tmp2);
  for (int i = 0; i < table[curtable].num_record; i++) {
    serecord[i] = and_or(type, tmp1[i], tmp2[i], 0);
  }
  skip_space();
  if (sql_lan[cline][0] == ';' || !strncmp(sql_lan[cline], "ORDER", 5) || sql_lan[cline][0] == ')') {
    return;
  } else {
    fail_flag = 1;
  }
}
void parse_conjunctable_clause(int curtable, int numrecord, int serecord[]) {
  int type = 1;
  int tmp1[128] = {0};
  parse_negatable_clause(curtable, table[curtable].num_record, tmp1);
  skip_space();
  if (!strncmp(sql_lan[cline], "AND", 3)) {
    sql_lan[cline] += 3;
    type = 2;
  } else {
    for (int i = 0; i < table[curtable].num_record; i++) {
      serecord[i] = tmp1[i];
    }
    return;
  }
  skip_space();
  int tmp2[128] = {0};
  parse_conjunctable_clause(curtable, table[curtable].num_record, tmp2);
  for (int i = 0; i < table[curtable].num_record; i++) {
    serecord[i] = and_or(type, tmp1[i], tmp2[i], 0);
  }
}
void parse_negatable_clause(int curtable, int numrecord, int serecord[]) {
  skip_space();
  int not = 0;
  if (!strncmp(sql_lan[cline], "NOT", 3)) {
    sql_lan[cline] += 3;
    skip_space();
    not = 1;
  }
  if (not == 1) {
    int tmp[128] = {0};
    parse_negatable_clause(curtable, table[curtable].num_record, tmp);
    for (int i = 0; i < numrecord; i++) {
      serecord[i] = and_or(1, 0, tmp[i], not );
    }
  } else {
    parse_automic_clause(curtable, table[curtable].num_record, serecord);
  }
}
void parse_automic_clause(int curtable, int numrecord, int serecord[]) {
  skip_space();
  if (sql_lan[cline][0] == '(') {
    sql_lan[cline]++;
    skip_space();
    parse_condition_clause(curtable, table[curtable].num_record, serecord);
    skip_space();
    if (sql_lan[cline][0] == ')') {
      sql_lan[cline]++;
      skip_space();
      return;
    } else {
      fail_flag = 1;
      return;
    }
  }
  int val[3] = {0};
  char *str[3] = {0};
  int curfield[3] = {0};
  int cmpo = 0;
  int sum = 0;
  int t[3] = {0};
  fail_flag = 0;
  get_data(curtable, val, str, curfield, &sum, t);
  if (fail_flag) {
    return;
  }
  skip_space();
  cmpo = check_cmpo();
  if (cmpo == -1) {
    fail_flag = 1;
    return;
  }
  if (fail_flag) {
    fail_flag = 0;
    skip_error_sen(2);
    return;
  }
  skip_space();
  if (cmpo == 7) {
    if (!strncmp(sql_lan[cline], "NULL", 4)) {
      sql_lan[cline] += 4;
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[0]].isnull[i] == 1) {
          serecord[i] = and_or(1, serecord[i], 1, 0);
        } else {
          serecord[i] = and_or(1, serecord[i], 0, 0);
        }
        // printf("=%d\n", serecord[i]);
      }
    } else if (!strncmp(sql_lan[cline], "NOT", 3)) {
      sql_lan[cline] += 3;
      skip_space();
      if (!strncmp(sql_lan[cline], "NULL", 4)) {
        sql_lan[cline] += 4;
        for (int i = 0; i < table[curtable].num_record; i++) {
          if (table[curtable].field[curfield[0]].isnull[i] == 0) {
            serecord[i] = and_or(1, serecord[i], 1, 0);
          } else {
            serecord[i] = and_or(1, serecord[i], 0, 0);
          }
        }
      } else {
        fail_flag = 1;
        return;
      }
    } else {
      fail_flag = 1;
      return;
    }
  } else if (cmpo == 8) {
    get_data(curtable, val, str, curfield, &sum, t);
    skip_space();
    if (!strncmp(sql_lan[cline], "AND", 3)) {
      sql_lan[cline] += 3;
    } else {
      skip_error_sen(1);
      return;
    }
    skip_space();
    get_data(curtable, val, str, curfield, &sum, t);
    if (t[0] == 3 && t[1] == 1 && t[2] == 1) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[0]].isnull[i] == 1)
          serecord[i] = -1;
        else {
          int tmp1 = cmporesint(6, table[curtable].field[curfield[0]].val[i], val[1]);
          int tmp2 = cmporesint(5, table[curtable].field[curfield[0]].val[i], val[2]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 1 && t[1] == 1 && t[2] == 3) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[2]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesint(6, val[0], val[1]);
          int tmp2 = cmporesint(5, val[0], table[curtable].field[curfield[2]].val[i]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 1 && t[1] == 3 && t[2] == 1) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[1]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesint(6, val[0], table[curtable].field[curfield[1]].val[i]);
          int tmp2 = cmporesint(5, val[0], val[2]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 1 && t[1] == 3 && t[2] == 3) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[1]].isnull[i] == 1 || table[curtable].field[curfield[2]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesint(6, val[0], table[curtable].field[curfield[1]].val[i]);
          int tmp2 = cmporesint(5, val[0], table[curtable].field[curfield[2]].val[i]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 3 && t[1] == 3 && t[2] == 1) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[1]].isnull[i] == 1 || table[curtable].field[curfield[0]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesint(6, table[curtable].field[curfield[0]].val[i], table[curtable].field[curfield[1]].val[i]);
          int tmp2 = cmporesint(5, table[curtable].field[curfield[0]].val[i], val[2]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 3 && t[1] == 1 && t[2] == 3) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[0]].isnull[i] == 1 || table[curtable].field[curfield[2]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesint(6, table[curtable].field[curfield[0]].val[i], val[1]);
          int tmp2 = cmporesint(5, table[curtable].field[curfield[0]].val[i], table[curtable].field[curfield[2]].val[i]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 1 && t[1] == 1 && t[2] == 1) {
      if (val[0] >= val[1] && val[0] <= val[2]) {
        for (int i = 0; i < table[curtable].num_record; i++) {
          serecord[i] = and_or(1, 0, 1, 0);
        }
      }
    } else if (t[0] == 3 && t[1] == 2 && t[2] == 2) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[0]].isnull[i] == 1)
          serecord[i] = -1;
        else {
          int tmp1 = cmporesstr(6, table[curtable].field[curfield[0]].str[i], str[1]);
          int tmp2 = cmporesstr(5, table[curtable].field[curfield[0]].str[i], str[2]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 2 && t[1] == 2 && t[2] == 3) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[2]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesstr(6, str[0], str[1]);
          int tmp2 = cmporesstr(5, str[0], table[curtable].field[curfield[2]].str[i]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 2 && t[1] == 3 && t[2] == 2) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[1]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesstr(6, str[0], table[curtable].field[curfield[1]].str[i]);
          int tmp2 = cmporesstr(5, str[0], str[2]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 2 && t[1] == 3 && t[2] == 3) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[1]].isnull[i] == 1 || table[curtable].field[curfield[2]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesstr(6, str[0], table[curtable].field[curfield[1]].str[i]);
          int tmp2 = cmporesstr(5, str[0], table[curtable].field[curfield[2]].str[i]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 3 && t[1] == 3 && t[2] == 2) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[0]].isnull[i] == 1 || table[curtable].field[curfield[1]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesstr(6, table[curtable].field[curfield[0]].str[i], table[curtable].field[curfield[1]].str[i]);
          int tmp2 = cmporesstr(5, table[curtable].field[curfield[0]].str[i], str[2]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 3 && t[1] == 2 && t[2] == 3) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[0]].isnull[i] == 1 || table[curtable].field[curfield[2]].isnull[i] == 1) {
          serecord[i] = -1;
        } else {
          int tmp1 = cmporesstr(6, table[curtable].field[curfield[0]].str[i], str[1]);
          int tmp2 = cmporesstr(5, table[curtable].field[curfield[0]].str[i], table[curtable].field[curfield[2]].str[i]);
          serecord[i] = and_or(2, tmp1, tmp2, 0);
          serecord[i] = and_or(1, 0, serecord[i], 0);
        }
      }
    } else if (t[0] == 3 && t[1] == 3 && t[2] == 3) {
      if (table[curtable].field[curfield[0]].type == table[curtable].field[curfield[1]].type &&
          table[curtable].field[curfield[0]].type == table[curtable].field[curfield[2]].type) {
        for (int i = 0; i < table[curtable].num_record; i++) {
          if (table[curtable].field[curfield[0]].type == 1) {
            if (table[curtable].field[curfield[0]].isnull[i] == 1 || table[curtable].field[curfield[1]].isnull[i] == 1 ||
                table[curtable].field[curfield[0]].isnull[i] == 1) {
              serecord[i] = -1;
            } else {
              int tmp1 = cmporesint(6, table[curtable].field[curfield[0]].val[i], table[curtable].field[curfield[1]].val[i]);
              int tmp2 = cmporesint(5, table[curtable].field[curfield[0]].val[i], table[curtable].field[curfield[2]].val[i]);
              serecord[i] = and_or(2, tmp1, tmp2, 0);
              serecord[i] = and_or(1, 0, serecord[i], 0);
            }
          } else if (table[curtable].field[curfield[0]].type == 2) {
            if (table[curtable].field[curfield[0]].isnull[i] == 1 || table[curtable].field[curfield[1]].isnull[i] == 1 ||
                table[curtable].field[curfield[0]].isnull[i] == 1) {
              serecord[i] = -1;
            } else {
              int tmp1 = cmporesstr(6, table[curtable].field[curfield[0]].str[i], table[curtable].field[curfield[1]].str[i]);
              int tmp2 = cmporesstr(5, table[curtable].field[curfield[0]].str[i], table[curtable].field[curfield[2]].str[i]);
              serecord[i] = and_or(2, tmp1, tmp2, 0);
              serecord[i] = and_or(1, 0, serecord[i], 0);
            }
          }
        }
      } else {
        e = 1;
        return;
      }
    } else if (t[0] == 2 && t[1] == 2 && t[2] == 2) {
      if (strcmp(str[0], str[1]) >= 0 && strcmp(str[0], str[2]) <= 0) {
        for (int i = 0; i < table[curtable].num_record; i++)
          serecord[i] = and_or(1, 0, 1, 0);
      }
    } else {
      e = 1;
      return;
    }
  } else {
    get_data(curtable, val, str, curfield, &sum, t);
    if (fail_flag) {
      return;
    }
    if (t[0] == 3 && t[1] == 1) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[0]].isnull[i])
          serecord[i] = and_or(1, serecord[i], -1, 0);
        else
          serecord[i] = and_or(1, serecord[i], cmporesint(cmpo, table[curtable].field[curfield[0]].val[i], val[1]), 0);
      }
    } else if (t[0] == 3 && t[1] == 2) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[0]].isnull[i] == 1) {
          serecord[i] = and_or(1, serecord[i], -1, 0);
        } else {
          // printf("%d==%d==%d==%d", type, serecord[i], cmporesstr(cmpo, table[curtable].field[curfield[0]].str[i], str[1]), not );
          serecord[i] = and_or(1, serecord[i], cmporesstr(cmpo, table[curtable].field[curfield[0]].str[i], str[1]), 0);
          // printf("==%d\n", serecord[i]);
        }
      }
    } else if (t[0] == 1 && t[1] == 3) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[1]].isnull[i])
          serecord[i] = and_or(1, serecord[i], -1, 0);
        else
          serecord[i] = and_or(1, serecord[i], cmporesint(cmpo, val[1], table[curtable].field[curfield[0]].val[i]) == 1, 0);
      }
    } else if (t[0] == 2 && t[1] == 3) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].field[curfield[1]].isnull[i])
          serecord[i] = and_or(1, serecord[i], 1, 0);
        else
          serecord[i] = and_or(1, serecord[i], cmporesstr(cmpo, str[1], table[curtable].field[curfield[0]].str[i]), 0);
      }
    } else if (t[0] == 1 && t[1] == 1) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        serecord[i] = and_or(1, serecord[i], cmporesint(cmpo, val[0], val[1]), 0);
      }
    } else if (t[0] == 2 && t[1] == 2) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        serecord[i] = and_or(1, serecord[i], cmporesstr(cmpo, str[0], str[1]), 0);
      }
    } else if (t[0] == 3 && t[1] == 3) {
      if (table[curtable].field[curfield[0]].type == table[curtable].field[curfield[1]].type) {
        if (table[curtable].field[curfield[0]].type == 1) {
          for (int i = 0; i < table[curtable].num_record; i++) {
            if (table[curtable].field[curfield[1]].isnull[i] || table[curtable].field[curfield[1]].isnull[i])
              serecord[i] = and_or(1, serecord[i], -1, 0);
            else
              serecord[i] = and_or(1, serecord[i], cmporesint(cmpo, table[curtable].field[curfield[0]].val[i], table[curtable].field[curfield[1]].val[i]), 0);
          }
        } else if (table[curtable].field[curfield[0]].type == 2) {
          for (int i = 0; i < table[curtable].num_record; i++) {
            if (table[curtable].field[curfield[1]].isnull[i] || table[curtable].field[curfield[1]].isnull[i])
              serecord[i] = and_or(1, serecord[i], -1, 0);
            else
              serecord[i] = and_or(1, serecord[i], cmporesstr(cmpo, table[curtable].field[curfield[0]].str[i], table[curtable].field[curfield[1]].str[i]), 0);
          }
        }
      } else {
        e = 1;
        return;
      }
    } else {
      e = 1;
      return;
    }
  }
}
void parse_wrong_condition_clause() {
  while (1) {
    if (!strncmp(sql_lan[cline], "AND", 3)) {
      sql_lan[cline] += 3;
    } else if (!strncmp(sql_lan[cline], "OR", 2)) {
      sql_lan[cline] += 2;
    }
    skip_space();
    if (!strncmp(sql_lan[cline], "NOT", 3)) {
      sql_lan[cline] += 3;
    }
    skip_space();
    if (sql_lan[cline][0] == '(') {
      sql_lan[cline]++;
      skip_space();
      int tmp[128] = {0};
      parse_wrong_condition_clause();
      if (sql_lan[cline][0] == ')') {
        sql_lan[cline]++;
        skip_space();
        if (sql_lan[cline][0] == ';')
          return;
        else if (!strncmp(sql_lan[cline], "ORDER", 5))
          return;
        else if (sql_lan[cline][0] == ')')
          return;
        else
          continue;
      }
    }
    int val[3] = {0};
    char *str[3] = {0};
    int curfield[3] = {0};
    int cmpo = 0;
    int sum = 0;
    int t[3] = {0};
    get_data(-1, val, str, curfield, &sum, t);
    if (fail_flag) {
      return;
    }
    skip_space();
    cmpo = check_cmpo();
    if (cmpo == -1) {
      fail_flag = 1;
      return;
    }
    if (fail_flag) {
      fail_flag = 0;
      skip_error_sen(2);
      return;
    }
    skip_space();
    if (cmpo == 7) {
      if (!strncmp(sql_lan[cline], "NULL", 4)) {
        sql_lan[cline] += 4;
      } else if (!strncmp(sql_lan[cline], "NOT", 3)) {
        sql_lan[cline] += 3;
        skip_space();
        if (!strncmp(sql_lan[cline], "NULL", 4)) {
          sql_lan[cline] += 4;
        } else {
          skip_error_sen(1);
          fail_flag = 1;
          return;
        }
      } else {
        skip_error_sen(1);
        fail_flag = 1;
        return;
      }
    } else if (cmpo == 8) {
      get_data(-1, val, str, curfield, &sum, t);
      skip_space();
      if (!strncmp(sql_lan[cline], "AND", 3)) {
        sql_lan[cline] += 3;
      }
      skip_space();
      get_data(-1, val, str, curfield, &sum, t);
      if (t[0] == 3 && t[1] == 1 && t[2] == 1) {
      } else if (t[0] == 3 && t[1] == 2 && t[2] == 2) {
      } else if (t[0] == 1 && t[1] == 3 && t[2] == 3) {
      } else {
        skip_error_sen(1);
        fail_flag = 1;
        return;
      }
    } else {
      get_data(-1, val, str, curfield, &sum, t);
      if (fail_flag) {
        return;
      }
      if (t[0] == 3 && t[1] == 1) {
      } else if (t[0] == 3 && t[1] == 2) {
      } else if (t[0] == 1 && t[1] == 3) {
      } else if (t[0] == 2 && t[1] == 3) {
      } else if (t[0] == 1 && t[1] == 1) {
      } else if (t[0] == 2 && t[1] == 2) {
      } else if (t[0] == 3 && t[1] == 3) {
      } else {
        fail_flag = 1;
        return;
      }
    }
    skip_space();
    if (sql_lan[cline][0] == ';' || !strncmp(sql_lan[cline], "ORDER", 5) || sql_lan[cline][0] == ')') {
      return;
    }
  }
}
void parse_wrong_where_and_order() {
  if (!strncmp(sql_lan[cline], "WHERE", 5)) {
    sql_lan[cline] += 5;
    skip_space();
    if (sql_lan[cline][0] == ';') {
      fail_flag = 1;
      return;
    }
    fail_flag = 0;
    parse_wrong_condition_clause();
    if (fail_flag) {
      return;
    }
    if (sql_lan[cline][0] == ';') {
      return;
    }
  }
  if (!strncmp(sql_lan[cline], "ORDER", 5)) {
    sql_lan[cline] += 5;
    skip_space();
    if (!strncmp(sql_lan[cline], "BY", 2)) {
      sql_lan[cline] += 2;
      skip_space();
      //
      int rank_type[100] = {0};
      int curfield[10] = {0};
      int sum_rank_field = 0;
      int rank_field[100] = {0};
      while (sql_lan[cline][0] != ';') {
        // ��ȡ�ֶ���
        char *tarfield = malloc(33);
        int len = 0;
        while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
          tarfield[len++] = *sql_lan[cline]++;
        }
        if (len == 0) {
          fail_flag = 1;
          return;
        }
        tarfield[len] = '\0';
        // ȡ�ֶ�������
        if (tarfield != NULL)
          free(tarfield);
        skip_space();
        if (!strncmp(sql_lan[cline], "ASC", 3)) {
          sql_lan[cline] += 3;
          skip_space();
        } else if (!strncmp(sql_lan[cline], "DESC", 4)) {
          sql_lan[cline] += 4;
          rank_type[sum_rank_field] = 1;
          skip_space();
        }
        if (sql_lan[cline][0] == ';') {
          sum_rank_field++;
          break;
        } else if (sql_lan[cline][0] == ',') {
          sql_lan[cline]++;
          skip_space();
          sum_rank_field++;
          continue;
        }
      }
      // ���
      // for (int i = 0; i < table[curtable].num_record; i++) {
      //   printf("%s\n", table[curtable].field[rank_field[0]].str[i]);
      // }
    } else {
      fail_flag = 1;
      return;
    }
  } else {
    skip_space();
    if (sql_lan[cline][0] != ';') {
      fail_flag = 1;
      return;
    }
  }
}
void get_data(int curtable, int val[], char *str[], int curfield[], int *sum, int t[]) {
  if (isdigit(sql_lan[cline][0]) || sql_lan[cline][0] == '-') {  // int
    t[*sum] = 1;
    val[(*sum)++] = atoi(sql_lan[cline]);
    sql_lan[cline]++;
    while (isdigit(sql_lan[cline][0])) {
      sql_lan[cline]++;
    }
  } else if (sql_lan[cline][0] == '\'') {  // char()
    t[*sum] = 2;
    char *tem = (char *)malloc(1024);
    sql_lan[cline]++;
    int len = 0;
    while (sql_lan[cline][0] != '\'') {
      if (sql_lan[cline][0] == '\\' && sql_lan[cline][1] == '\'') {
        tem[len++] = sql_lan[cline][0];
        tem[len++] = sql_lan[cline][1];
        sql_lan[cline] += 2;
      } else {
        tem[len++] = *sql_lan[cline]++;
      }
    }
    tem[len] = 0;
    sql_lan[cline]++;
    str[*sum] = malloc(strlen(tem));
    strcpy(str[*sum], tem);
    (*sum)++;
    if (tem != NULL)
      free(tem);
  } else {  // tarfield
    t[*sum] = 3;
    // ��ȡ�ֶ���
    char *tarfield = malloc(33);
    int len = 0;
    while (isalpha(sql_lan[cline][0]) || sql_lan[cline][0] == '_') {
      tarfield[len++] = *sql_lan[cline]++;
    }
    // if (len == 0) {
    //   fail_flag = 1;
    //   skip_error_sen(1);
    //   return;
    // }
    tarfield[len] = 0;
    if (curtable != -1)
      curfield[(*sum)++] = is_exist_table_field(curtable, tarfield);
    else
      (*sum)++;
    free(tarfield);
    if (curtable != -1 && curfield[*sum - 1] < 0) {
      fail_flag = 1;
      return;
    }
    // ȡ�ֶ�������
  }
}
int check_cmpo() {
  if (!strncmp(sql_lan[cline], "=", 1)) {
    sql_lan[cline]++;
    return 1;
  } else if (!strncmp(sql_lan[cline], "<>", 2)) {
    sql_lan[cline] += 2;
    return 2;
  } else if (!strncmp(sql_lan[cline], "<=", 2)) {
    sql_lan[cline] += 2;
    return 5;
  } else if (!strncmp(sql_lan[cline], ">=", 2)) {
    sql_lan[cline] += 2;
    return 6;
  } else if (!strncmp(sql_lan[cline], "<", 1)) {
    sql_lan[cline]++;
    return 3;
  } else if (!strncmp(sql_lan[cline], ">", 1)) {
    sql_lan[cline]++;
    return 4;
  } else if (!strncmp(sql_lan[cline], "IS", 2)) {
    sql_lan[cline] += 2;
    return 7;
  } else if (!strncmp(sql_lan[cline], "BETWEEN", 7)) {
    sql_lan[cline] += 7;
    return 8;
  } else {
    return -1;
  }
}
int and_or(int type, int val, int cur, int not ) {
  if (type == 1 && not == 0 && cur == 1) { return 1; }
  if (type == 1 && not == 1 && cur == 0) { return 1; }
  if (type == 1 && cur == -1) { return -1; }
  if (type == 2 && not == 0 && val == 1 && cur == 1) { return 1; }
  if (type == 2 && not == 1 && val == 1 && cur == 0) { return 1; }
  if (type == 2 && ((val == 1 && cur == -1) || (val == -1 && cur == 0 && not == 1) || (val == -1 && cur == 1 && not == 0))) { return -1; }
  if (type == 3 && not == 0 && (val == 1 || cur == 1)) { return 1; }
  if (type == 3 && not == 1 && (val == 1 || cur == 0)) { return 1; }
  if (type == 3 && ((val == -1 && cur == -1) || (val == -1 && cur == 1 && not == 1) || (val == -1 && cur == 0 && not == 0) || (val == 0 && cur == -1))) { return -1; }
  return 0;
}
int cmporesint(int cmpo, int v1, int v2) {
  if ((cmpo == 1 && v1 == v2) || (cmpo == 2 && v1 != v2) || (cmpo == 3 && v1 < v2) || (cmpo == 4 && v1 > v2) || (cmpo == 5 && v1 <= v2) || (cmpo == 6 && v1 >= v2)) {
    return 1;
  } else if ((cmpo == 1 && v1 != v2) || (cmpo == 2 && v1 == v2) || (cmpo == 3 && v1 >= v2) || (cmpo == 4 && v1 <= v2) || (cmpo == 5 && v1 > v2) || (cmpo == 6 && v1 < v2)) {
    return 0;
  } else {
    return -1;
  }
}
int cmporesstr(int cmpo, char *s1, char *s2) {
  if ((cmpo == 1 && !strcmp(s1, s2)) || (cmpo == 2 && strcmp(s1, s2) != 0) || (cmpo == 3 && strcmp(s1, s2) < 0) || (cmpo == 4 && strcmp(s1, s2) > 0) || (cmpo == 5 && strcmp(s1, s2) <= 0) || (cmpo == 6 && strcmp(s1, s2) >= 0)) {
    return 1;
  } else if ((cmpo == 1 && strcmp(s1, s2) != 0) || (cmpo == 2 && !strcmp(s1, s2)) || (cmpo == 3 && strcmp(s1, s2) >= 0) || (cmpo == 4 && strcmp(s1, s2) <= 0) || (cmpo == 5 && strcmp(s1, s2) > 0) || (cmpo == 6 && strcmp(s1, s2) < 0)) {
    return 0;
  }
  return -1;
}
void sort_prk(int curtable) {
  int prkfield = 0;
  for (int i = 0; i < table[curtable].num_field; i++) {
    if (table[curtable].field[i].is_prk == true) {
      prkfield = i;
      break;
    }
  }
  if (table[curtable].field[prkfield].type == 1) {
    for (int i = 0; i < table[curtable].num_record - 1; i++) {
      for (int j = i + 1; j < table[curtable].num_record; j++) {
        if (table[curtable].field[prkfield].isnull[i] == 1) {
          swap_two_records(curtable, i, j);
        } else if (table[curtable].field[prkfield].val[i] > table[curtable].field[prkfield].val[j] && table[curtable].field[prkfield].isnull[j] != true) {
          swap_two_records(curtable, i, j);
        }
      }
    }
  } else if (table[curtable].field[prkfield].type == 2) {
    for (int i = 0; i < table[curtable].num_record - 1; i++) {
      for (int j = i + 1; j < table[curtable].num_record; j++) {
        if (table[curtable].field[prkfield].isnull[i] == 1) {
          swap_two_records(curtable, i, j);
        } else if (strcmp(table[curtable].field[prkfield].str[i], table[curtable].field[prkfield].str[j]) > 0 && table[curtable].field[prkfield].isnull[j] != true) {
          swap_two_records(curtable, i, j);
        }
      }
    }
  }
}
void print_table(int curtable, int curfield[], int sumfield) {
  int sum = 0;
  for (int i = 0; i < table[curtable].num_record; i++) {
    if (table[curtable].serecord[i] == 1) {
      sum++;
    }
  }
  printf("%d RECORDS FOUND\n", sum);
  if (sum) {
    if (curfield == NULL) {
      for (int i = 0; i < table[curtable].num_field; i++) {
        printf("%s\t", table[curtable].field[i].name);
      }
    } else {
      for (int i = 0; i < sumfield; i++) {
        printf("%s\t", table[curtable].field[curfield[i]].name);
      }
    }
    printf("\n");

    if (curfield == NULL) {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].serecord[i] == 1) {
          table[curtable].serecord[i] = 0;
          for (int j = 0; j < table[curtable].num_field; j++) {
            if (table[curtable].field[j].type == 1) {
              if (table[curtable].field[j].isnull[i] == 1) {
                printf("NULL\t");
              } else {
                printf("%d\t", table[curtable].field[j].val[i]);
              }
            } else if (table[curtable].field[j].type == 2) {
              if (table[curtable].field[j].isnull[i] == 1) {
                printf("NULL\t");
              } else {
                printf("\'%s\'\t", table[curtable].field[j].str[i]);
              }
            }
          }
          printf("\n");
        }
      }
    } else {
      for (int i = 0; i < table[curtable].num_record; i++) {
        if (table[curtable].serecord[i] == 1) {
          table[curtable].serecord[i] = 0;
          for (int j = 0; j < sumfield; j++) {
            if (table[curtable].field[curfield[j]].type == 1) {
              if (table[curtable].field[curfield[j]].isnull[i] == 1) {
                printf("NULL\t");
              } else {
                printf("%d\t", table[curtable].field[curfield[j]].val[i]);
              }
            } else if (table[curtable].field[curfield[j]].type == 2) {
              if (table[curtable].field[curfield[j]].isnull[i] == 1) {
                printf("NULL\t");
              } else {
                printf("\'%s\'\t", table[curtable].field[curfield[j]].str[i]);
              }
            }
          }
          printf("\n");
        }
      }
    }
  }
}
void sort_field(int curtable, int rank_field[], int rank_type[], int sum_rank_field) {
  int prkfield = 0;
  for (int i = 0; i < table[curtable].num_field; i++) {
    if (table[curtable].field[i].is_prk == true) {
      prkfield = i;
      break;
    }
  }
  for (int r = 0; r < sum_rank_field; r++) {
    int flag = 1;
    if (r == 0) {
      if (rank_type[r] == 0) {
        if (table[curtable].field[rank_field[0]].type == 1) {
          for (int i = 0; i < table[curtable].num_record - 1; i++) {
            for (int j = i + 1; j < table[curtable].num_record; j++) {
              if (table[curtable].field[rank_field[0]].isnull[i] == 1 && table[curtable].field[rank_field[0]].isnull[j] != 1) {
                swap_two_records(curtable, i, j);
                flag = 0;
              } else if (table[curtable].field[rank_field[0]].isnull[j] != true && table[curtable].field[rank_field[0]].val[i] > table[curtable].field[rank_field[0]].val[j]) {
                swap_two_records(curtable, i, j);
                flag = 0;
              }
            }
          }
        } else if (table[curtable].field[rank_field[0]].type == 2) {
          for (int i = 0; i < table[curtable].num_record - 1; i++) {
            for (int j = i + 1; j < table[curtable].num_record; j++) {
              if (table[curtable].field[rank_field[0]].isnull[i] == 1 && table[curtable].field[rank_field[0]].isnull[j] != 1) {
                swap_two_records(curtable, i, j);
                flag = 0;
              } else if (table[curtable].field[rank_field[0]].isnull[j] == 0 && strcmp(table[curtable].field[rank_field[0]].str[i], table[curtable].field[rank_field[0]].str[j]) > 0) {
                swap_two_records(curtable, i, j);
                flag = 0;
              }
            }
          }
        }
      } else if (rank_type[r] == 1) {
        if (table[curtable].field[rank_field[0]].type == 1) {
          for (int i = 0; i < table[curtable].num_record - 1; i++) {
            for (int j = i + 1; j < table[curtable].num_record; j++) {
              if (table[curtable].field[rank_field[0]].isnull[i] == 1 && table[curtable].field[rank_field[0]].isnull[j] != 1) {
                swap_two_records(curtable, i, j);
                flag = 0;
              } else if (table[curtable].field[rank_field[0]].isnull[j] != true && table[curtable].field[rank_field[0]].val[i] < table[curtable].field[rank_field[0]].val[j]) {
                swap_two_records(curtable, i, j);
                flag = 0;
              }
            }
          }
        } else if (table[curtable].field[rank_field[0]].type == 2) {
          for (int i = 0; i < table[curtable].num_record - 1; i++) {
            for (int j = i + 1; j < table[curtable].num_record; j++) {
              if (table[curtable].field[rank_field[0]].isnull[i] == 1 && table[curtable].field[rank_field[0]].isnull[j] != 1) {
                swap_two_records(curtable, i, j);
                flag = 0;
              } else if (table[curtable].field[rank_field[0]].isnull[j] == 0 && strcmp(table[curtable].field[rank_field[0]].str[i], table[curtable].field[rank_field[0]].str[j]) < 0) {
                swap_two_records(curtable, i, j);
                flag = 0;
              }
            }
          }
        }
      }
    } else {
      if (rank_type[r] == 0) {
        if (table[curtable].field[rank_field[r]].type == 1) {
          for (int i = 0; i < table[curtable].num_record - 1; i++) {
            for (int j = i + 1; j < table[curtable].num_record; j++) {
              if (table[curtable].field[rank_field[r - 1]].isnull[i] == 1 && table[curtable].field[rank_field[r - 1]].isnull[j] == 1) {
                if (table[curtable].field[rank_field[r]].isnull[i] == 1 && table[curtable].field[rank_field[r]].isnull[j] == 0) {
                  swap_two_records(curtable, i, j);
                  flag = 0;
                } else if (table[curtable].field[rank_field[r]].isnull[j] == 0 && table[curtable].field[rank_field[r]].val[i] > table[curtable].field[rank_field[r]].val[j]) {
                  swap_two_records(curtable, i, j);
                  flag = 0;
                }
              } else if (table[curtable].field[rank_field[r - 1]].isnull[i] == 0 && table[curtable].field[rank_field[r - 1]].isnull[j] == 0) {
                if ((table[curtable].field[rank_field[r - 1]].type == 1 && table[curtable].field[rank_field[r - 1]].val[i] == table[curtable].field[rank_field[r - 1]].val[j]) ||
                    (table[curtable].field[rank_field[r - 1]].type == 2 && !strcmp(table[curtable].field[rank_field[r - 1]].str[i], table[curtable].field[rank_field[r - 1]].str[j]))) {
                  if (table[curtable].field[rank_field[r]].isnull[i] == 1 && table[curtable].field[rank_field[r]].isnull[j] == 0) {
                    swap_two_records(curtable, i, j);
                    flag = 0;
                  } else if (table[curtable].field[rank_field[r]].isnull[i] == 0 && table[curtable].field[rank_field[r]].isnull[j] == 0 && table[curtable].field[rank_field[r]].val[i] > table[curtable].field[rank_field[r]].val[j]) {
                    swap_two_records(curtable, i, j);
                    flag = 0;
                  }
                }
              }
            }
          }
        } else if (table[curtable].field[rank_field[r]].type == 2) {
          for (int i = 0; i < table[curtable].num_record - 1; i++) {
            for (int j = i + 1; j < table[curtable].num_record; j++) {
              if (table[curtable].field[rank_field[r - 1]].isnull[i] == 1 && table[curtable].field[rank_field[r - 1]].isnull[j] == 1) {
                if (table[curtable].field[rank_field[r]].isnull[i] == 1 && table[curtable].field[rank_field[r]].isnull[j] == 0) {
                  swap_two_records(curtable, i, j);
                  flag = 0;
                } else if (table[curtable].field[rank_field[r]].isnull[i] == 0 && table[curtable].field[rank_field[r]].isnull[j] == 0 && strcmp(table[curtable].field[rank_field[r]].str[i], table[curtable].field[rank_field[r]].str[j]) > 0) {
                  swap_two_records(curtable, i, j);
                  flag = 0;
                }
              } else if (table[curtable].field[rank_field[r - 1]].isnull[i] == 0 && table[curtable].field[rank_field[r - 1]].isnull[j] == 0) {
                if ((table[curtable].field[rank_field[r - 1]].type == 1 && table[curtable].field[rank_field[r - 1]].val[i] == table[curtable].field[rank_field[r - 1]].val[j]) ||
                    (table[curtable].field[rank_field[r - 1]].type == 2 && !strcmp(table[curtable].field[rank_field[r - 1]].str[i], table[curtable].field[rank_field[r - 1]].str[j]))) {
                  if (table[curtable].field[rank_field[r]].isnull[i] == 1 && table[curtable].field[rank_field[r]].isnull[j] == 0) {
                    swap_two_records(curtable, i, j);
                    flag = 0;
                  } else if (table[curtable].field[rank_field[r]].isnull[i] == 0 && table[curtable].field[rank_field[r]].isnull[j] == 0 && strcmp(table[curtable].field[rank_field[r]].str[i], table[curtable].field[rank_field[r]].str[j]) > 0) {
                    swap_two_records(curtable, i, j);
                    flag = 0;
                  }
                }
              }
            }
          }
        }
      } else if (rank_type[r] == 1) {
        if (table[curtable].field[rank_field[r]].type == 1) {
          for (int i = 0; i < table[curtable].num_record - 1; i++) {
            for (int j = i + 1; j < table[curtable].num_record; j++) {
              if (table[curtable].field[rank_field[r - 1]].isnull[i] == 1 && table[curtable].field[rank_field[r - 1]].isnull[j] == 1) {
                if (table[curtable].field[rank_field[r]].isnull[i] == 1 && table[curtable].field[rank_field[r]].isnull[j] == 0) {
                  swap_two_records(curtable, i, j);
                  flag = 0;
                } else if (table[curtable].field[rank_field[r]].isnull[i] == 0 && table[curtable].field[rank_field[r]].isnull[j] == 0 && table[curtable].field[rank_field[r]].val[i] < table[curtable].field[rank_field[r]].val[j]) {
                  swap_two_records(curtable, i, j);
                  flag = 0;
                }
              } else if (table[curtable].field[rank_field[r - 1]].isnull[i] == 0 && table[curtable].field[rank_field[r - 1]].isnull[j] == 0) {
                if ((table[curtable].field[rank_field[r - 1]].type == 1 && table[curtable].field[rank_field[r - 1]].val[i] == table[curtable].field[rank_field[r - 1]].val[j]) ||
                    (table[curtable].field[rank_field[r - 1]].type == 2 && !strcmp(table[curtable].field[rank_field[r - 1]].str[i], table[curtable].field[rank_field[r - 1]].str[j]))) {
                  if (table[curtable].field[rank_field[r]].isnull[i] == 1 && table[curtable].field[rank_field[r]].isnull[j] == 0) {
                    swap_two_records(curtable, i, j);
                    flag = 0;
                  } else if (table[curtable].field[rank_field[r]].isnull[i] == 0 && table[curtable].field[rank_field[r]].isnull[j] == 0 && table[curtable].field[rank_field[r]].val[i] < table[curtable].field[rank_field[r]].val[j]) {
                    swap_two_records(curtable, i, j);
                    flag = 0;
                  }
                }
              }
            }
          }
        } else if (table[curtable].field[rank_field[r]].type == 2) {
          for (int i = 0; i < table[curtable].num_record - 1; i++) {
            for (int j = i + 1; j < table[curtable].num_record; j++) {
              if (table[curtable].field[rank_field[r - 1]].isnull[i] && table[curtable].field[rank_field[r - 1]].isnull[j]) {
                if (table[curtable].field[rank_field[r]].isnull[i] == 1 && table[curtable].field[rank_field[r]].isnull[j] == 0) {
                  swap_two_records(curtable, i, j);
                  flag = 0;
                } else if (table[curtable].field[rank_field[r]].isnull[i] == 0 && table[curtable].field[rank_field[r]].isnull[j] == 0 && strcmp(table[curtable].field[rank_field[r]].str[i], table[curtable].field[rank_field[r]].str[j]) < 0) {
                  swap_two_records(curtable, i, j);
                  flag = 0;
                }
              } else if (table[curtable].field[rank_field[r - 1]].isnull[i] == 0 && table[curtable].field[rank_field[r - 1]].isnull[j] == 0) {
                if ((table[curtable].field[rank_field[r - 1]].type == 1 && table[curtable].field[rank_field[r - 1]].val[i] == table[curtable].field[rank_field[r - 1]].val[j]) ||
                    (table[curtable].field[rank_field[r - 1]].type == 2 && !strcmp(table[curtable].field[rank_field[r - 1]].str[i], table[curtable].field[rank_field[r - 1]].str[j]))) {
                  if (table[curtable].field[rank_field[r]].isnull[i] == 1 && table[curtable].field[rank_field[r]].isnull[j] == 0) {
                    swap_two_records(curtable, i, j);
                    flag = 0;
                  } else if (table[curtable].field[rank_field[r]].isnull[i] == 0 && table[curtable].field[rank_field[r]].isnull[j] == 0 && strcmp(table[curtable].field[rank_field[r]].str[i], table[curtable].field[rank_field[r]].str[j]) < 0) {
                    swap_two_records(curtable, i, j);
                    flag = 0;
                  }
                }
              }
            }
          }
        }
      }
    }
    if (flag) { break; }
  }
  if (table[curtable].field[prkfield].type == 1) {
    for (int i = 0; i < table[curtable].num_record - 1; i++) {
      for (int j = i + 1; j < table[curtable].num_record; j++) {
        int s = 1;
        for (int m = 0; m < sum_rank_field; m++) {
          if (!((table[curtable].field[rank_field[m]].isnull[i] && table[curtable].field[rank_field[m]].isnull[j]) ||
                (table[curtable].field[rank_field[m]].isnull[i] == 0 && table[curtable].field[rank_field[m]].isnull[j] == 0 && table[curtable].field[rank_field[m]].type == 1 && table[curtable].field[rank_field[m]].val[i] == table[curtable].field[rank_field[m]].val[j]) ||
                (table[curtable].field[rank_field[m]].isnull[i] == 0 && table[curtable].field[rank_field[m]].isnull[j] == 0 && table[curtable].field[rank_field[m]].type == 2 && !strcmp(table[curtable].field[rank_field[m]].str[i], table[curtable].field[rank_field[m]].str[j])))) {
            s = 0;
            break;
          }
        }
        if (s) {
          if (table[curtable].field[prkfield].val[i] > table[curtable].field[prkfield].val[j]) {
            swap_two_records(curtable, i, j);
          }
        }
      }
    }
  } else if (table[curtable].field[prkfield].type == 2) {
    for (int i = 0; i < table[curtable].num_record - 1; i++) {
      for (int j = i + 1; j < table[curtable].num_record; j++) {
        int s = 1;
        for (int m = 0; m < sum_rank_field; m++) {
          if (!((table[curtable].field[rank_field[m]].isnull[i] && table[curtable].field[rank_field[m]].isnull[j]) || (table[curtable].field[rank_field[m]].isnull[i] == 0 && table[curtable].field[rank_field[m]].isnull[j] == 0 && table[curtable].field[rank_field[m]].type == 1 && table[curtable].field[rank_field[m]].val[i] == table[curtable].field[rank_field[m]].val[j]) || (table[curtable].field[rank_field[m]].isnull[i] == 0 && table[curtable].field[rank_field[m]].isnull[j] == 0 && table[curtable].field[rank_field[m]].type == 2 && !strcmp(table[curtable].field[rank_field[m]].str[i], table[curtable].field[rank_field[m]].str[j])))) {
            s = 0;
          }
        }
        if (s) {
          if (strcmp(table[curtable].field[prkfield].str[i], table[curtable].field[prkfield].str[j]) > 0) {
            swap_two_records(curtable, i, j);
          }
        }
      }
    }
  }
}
void swap_two_records(int curtable, int r1, int r2) {
  int tmp = table[curtable].serecord[r1];
  table[curtable].serecord[r1] = table[curtable].serecord[r2];
  table[curtable].serecord[r2] = tmp;
  for (int i = 0; i < table[curtable].num_field; i++) {
    char *tmp1 = table[curtable].field[i].str[r1];
    int tmp2 = table[curtable].field[i].val[r1];
    int tmp3 = table[curtable].field[i].isnull[r1];
    table[curtable].field[i].str[r1] = table[curtable].field[i].str[r2];
    table[curtable].field[i].val[r1] = table[curtable].field[i].val[r2];
    table[curtable].field[i].isnull[r1] = table[curtable].field[i].isnull[r2];
    table[curtable].field[i].str[r2] = tmp1;
    table[curtable].field[i].val[r2] = tmp2;
    table[curtable].field[i].isnull[r2] = tmp3;
  }
}
