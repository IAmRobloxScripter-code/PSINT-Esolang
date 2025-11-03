#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
  uint8_t registers[3];
  uint8_t current_register;
  char *output;
  uint64_t output_size;
} PSINT_VM;

typedef enum {
  loop,
  set,
  print,
  add,
  sub,
  shift_front,
  shift_back,
  input,
} kind;

typedef enum {
  number,
  please,
  homeless,
  need,
  with,
  help,
  live,
  kinda,
  my,
  identifier,
  comment,
  eof_t,

  open_brace,
  closing_brace,
} token_kind;

typedef struct token_t {
  token_kind token_type;
  char value[128];
} token_t;

token_t *tokenize(const char *source) {
  int capacity = 16;
  int position = 0;
  int count = 0;
  token_t *tokens = malloc(sizeof(token_t) * capacity);
  char word[128];

  while (source[position] != '\0') {
    char c = source[position];

    if (isspace(c)) {
      position++;
      continue;
    }

    if (count >= capacity) {
      capacity *= 2;
      tokens = realloc(tokens, sizeof(token_t) * capacity);
    }

    if (c == '{') {
      position++;
      tokens[count].token_type = open_brace;
      count++;
      continue;
    } else if (c == '}') {
      position++;
      tokens[count].token_type = closing_brace;
      count++;
      continue;
    }

    if (c == '#') {
      position++;
      int word_index = 0;
      while (source[position] != '\0' && source[position] != '\n' &&
             source[position] != '\r' && word_index < 127) {
        word[word_index++] = source[position++];
      };
      word[word_index] = '\0';
      continue;
    }

    if (isdigit(c)) {
      int number_index = 0;
      do {
        word[number_index++] = source[position++];
      } while (isdigit(source[position]) && number_index < 127);
      word[number_index] = '\0';
      tokens[count].token_type = number;
      strcpy(tokens[count].value, word);
      continue;
    }

    if (isalpha(c)) {
      int word_index = 0;
      do {
        word[word_index++] = source[position++];
      } while (isalnum(source[position]) && word_index < 127);
      word[word_index] = '\0';

      token_kind kind = identifier;
      if (!strcmp(word, "please"))
        kind = please;
      else if (!strcmp(word, "homeless"))
        kind = homeless;
      else if (!strcmp(word, "need"))
        kind = need;
      else if (!strcmp(word, "with"))
        kind = with;
      else if (!strcmp(word, "help"))
        kind = help;
      else if (!strcmp(word, "live"))
        kind = live;
      else if (!strcmp(word, "kinda"))
        kind = kinda;
      else if (!strcmp(word, "my"))
        kind = my;

      tokens[count].token_type = kind;
      strcpy(tokens[count].value, word);
      count++;
      continue;
    }

    position++;
  }

  tokens[count].token_type = eof_t;

  return tokens;
}

typedef struct node node;

typedef struct loop_node {
  node **body;
} loop_node;

typedef struct set_node {
  char *register_value;
} set_node;

typedef struct node {
  kind node_kind;
  struct loop_node loop_value;
  struct set_node set_value;
} node;

void funny_error() {
  fprintf(stderr,
          "please speed i need this, my mommas kinda homeless.. i live with "
          "my dad im tryna help her out. Speed im watching the stream why "
          "you tryna not to laugh bruh thats disrespectful as shit bruh.");
  return;
}

node *parse_stmt(token_t *tokens, uint64_t *count) {
  token_t token = tokens[*count];
  switch (token.token_type) {
  case please: {
    (*count)++;
    char *register_value = tokens[*count].value;
    (*count)++;
    set_node please_node = {};
    please_node.register_value = register_value;

    node *actual_node = malloc(sizeof(node));
    actual_node->set_value = please_node;
    actual_node->node_kind = set;
    return actual_node;
  };
  case need: {
    // need {}
    (*count) += 2;

    loop_node need_node = {};

    uint64_t ast_count = 0;
    uint64_t ast_capacity = 16;

    node **ast = malloc(sizeof(node *) * ast_capacity);

    while (tokens[*count].token_type != eof_t &&
           tokens[*count].token_type != closing_brace) {
      if (ast_count > ast_capacity) {
        ast_capacity *= 2;
        ast = realloc(ast, sizeof(node *) * ast_capacity);
      }

      ast[ast_count] = parse_stmt(tokens, count);
      ast_count++;
    }
    (*count)++;
    ast[ast_count] = NULL;
    need_node.body = ast;

    node *actual_node = malloc(sizeof(node));
    actual_node->node_kind = loop;
    actual_node->loop_value = need_node;
    return actual_node;
  };
  case live: {
    (*count)++;
    node *actual_node = malloc(sizeof(node));
    actual_node->node_kind = add;
    return actual_node;
  };
  case kinda: {
    (*count)++;
    node *actual_node = malloc(sizeof(node));
    actual_node->node_kind = sub;
    return actual_node;
  };
  case homeless: {
    (*count)++;
    node *actual_node = malloc(sizeof(node));
    actual_node->node_kind = print;
    return actual_node;
  };
  case with: {
    (*count)++;
    node *actual_node = malloc(sizeof(node));
    actual_node->node_kind = shift_front;
    return actual_node;
  };
  case help: {
    (*count)++;
    node *actual_node = malloc(sizeof(node));
    actual_node->node_kind = shift_back;
    return actual_node;
  };
  case my: {
    (*count)++;
    node *actual_node = malloc(sizeof(node));
    actual_node->node_kind = input;
    return actual_node;
  };
  default:
    funny_error();
    return 0;
  }
}

node **parse(const char *source) {
  token_t *tokens = tokenize(source);
  uint64_t ast_count = 0;
  uint64_t count = 0;
  uint64_t ast_capacity = 16;

  node **ast = malloc(sizeof(node *) * ast_capacity);

  while (tokens[count].token_type != eof_t) {
    if (ast_count > ast_capacity) {
      ast_capacity *= 2;
      ast = realloc(ast, sizeof(node *) * ast_capacity);
    }

    ast[ast_count] = parse_stmt(tokens, &count);
    ast_count++;
  }

  ast[ast_count] = NULL;

  return ast;
}

void execute(PSINT_VM *vm, node **ast, uint8_t step_mode);

void execute_node(PSINT_VM *vm, node *executable_node, uint8_t step_mode) {
  switch (executable_node->node_kind) {
  case set: {
    uint8_t id = 0;
    if (strcmp(executable_node->set_value.register_value, "mommas") == 0) {
      id = 0;
    } else if (strcmp(executable_node->set_value.register_value, "dad") == 0) {
      id = 1;
    } else if (strcmp(executable_node->set_value.register_value, "speed") ==
               0) {
      id = 2;
    } else {
      funny_error();
      return;
    }
    vm->current_register = id;
    return;
  };
  case add:
    vm->registers[vm->current_register]++;
    return;
  case sub:
    vm->registers[vm->current_register]--;
    return;
  case print:
    vm->output = realloc(vm->output, sizeof(char) * (vm->output_size + 2));
    vm->output[vm->output_size] = vm->registers[vm->current_register];
    vm->output[vm->output_size + 1] = '\0';
    vm->output_size++;
    return;
  case loop: {
    while (vm->registers[vm->current_register] != 0) {
      execute(vm, executable_node->loop_value.body, step_mode);
    };
    return;
  };
  case shift_front: {
    uint8_t A = vm->registers[0];
    uint8_t B = vm->registers[1];
    uint8_t C = vm->registers[2];

    vm->registers[0] = C;
    vm->registers[1] = A;
    vm->registers[2] = B;
    return;
  };
  case shift_back: {
    uint8_t A = vm->registers[0];
    uint8_t B = vm->registers[1];
    uint8_t C = vm->registers[2];

    vm->registers[0] = B;
    vm->registers[1] = C;
    vm->registers[2] = A;
    return;
  };
  case input: {
    char input_value;
    scanf(" %c", &input_value);
    vm->registers[vm->current_register] = input_value;
    return;
  };
  default:
    funny_error();
    return;
  };
}

void execute(PSINT_VM *vm, node **ast, uint8_t step_mode) {
  node **ptr = ast;

  while (*ptr != NULL) {
    if (step_mode == 1)
      getchar();
    execute_node(vm, *ptr, step_mode);
    ptr++;
    if (step_mode == 1)
      printf("Mommas: %d | Dad: %d | Speed: %d | Output: %s | Register: %d",
             vm->registers[0], vm->registers[1], vm->registers[2], vm->output,
             vm->current_register);
  }

  return;
}

char *read_file_as_string(const char *filename) {
  FILE *file = fopen(filename, "r");
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = malloc(file_size + 1);
  size_t bytes_read = fread(buffer, 1, file_size, file);
  buffer[file_size] = '\0';

  fclose(file);
  return buffer;
}

const char *run(const char *source, uint8_t step_mode) {
  node **ast = parse(source);

  PSINT_VM vm = {};
  vm.current_register = 0;
  vm.registers[0] = 0;
  vm.registers[1] = 0;
  vm.registers[2] = 0;
  vm.output_size = 0;
  vm.output = malloc(sizeof(char) * 1);
  execute(&vm, ast, step_mode);
  printf("Mommas: %d | Dad: %d | Speed: %d | Output: %s | Register: %d\n",
         vm.registers[0], vm.registers[1], vm.registers[2], vm.output,
         vm.current_register);

  vm.output[vm.output_size] = '\0';
  return vm.output;
}

int main(int argc, char **argv) {
  char *content = read_file_as_string(argv[1]);
  uint8_t step_mode = 0;
  if (argc > 2 && strcmp(argv[2], "-s") == 0)
    step_mode = 1;

  const char *output = run(content, step_mode);
  printf("%s\n", output);
  return 0;
}
