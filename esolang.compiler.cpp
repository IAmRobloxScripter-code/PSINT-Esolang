#include <cstddef>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

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
  token_t *tokens = (token_t *)malloc(sizeof(token_t) * capacity);
  char word[128];

  while (source[position] != '\0') {
    char c = source[position];

    if (isspace(c)) {
      position++;
      continue;
    }

    if (count >= capacity) {
      capacity *= 2;
      tokens = (token_t *)realloc(tokens, sizeof(token_t) * capacity);
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

    node *actual_node = (node *)malloc(sizeof(node));
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

    node **ast = (node **)malloc(sizeof(node *) * ast_capacity);

    while (tokens[*count].token_type != eof_t &&
           tokens[*count].token_type != closing_brace) {
      if (ast_count > ast_capacity) {
        ast_capacity *= 2;
        ast = (node **)realloc(ast, sizeof(node *) * ast_capacity);
      }

      ast[ast_count] = parse_stmt(tokens, count);
      ast_count++;
    }
    (*count)++;
    ast[ast_count] = NULL;
    need_node.body = ast;

    node *actual_node = (node *)malloc(sizeof(node));
    actual_node->node_kind = loop;
    actual_node->loop_value = need_node;
    return actual_node;
  };
  case live: {
    (*count)++;
    node *actual_node = (node *)malloc(sizeof(node));
    actual_node->node_kind = add;
    return actual_node;
  };
  case kinda: {
    (*count)++;
    node *actual_node = (node *)malloc(sizeof(node));
    actual_node->node_kind = sub;
    return actual_node;
  };
  case homeless: {
    (*count)++;
    node *actual_node = (node *)malloc(sizeof(node));
    actual_node->node_kind = print;
    return actual_node;
  };
  case with: {
    (*count)++;
    node *actual_node = (node *)malloc(sizeof(node));
    actual_node->node_kind = shift_front;
    return actual_node;
  };
  case help: {
    (*count)++;
    node *actual_node = (node *)malloc(sizeof(node));
    actual_node->node_kind = shift_back;
    return actual_node;
  };
  case my: {
    (*count)++;
    node *actual_node = (node *)malloc(sizeof(node));
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

  node **ast = (node **)malloc(sizeof(node *) * ast_capacity);

  while (tokens[count].token_type != eof_t) {
    if (ast_count > ast_capacity) {
      ast_capacity *= 2;
      ast = (node **)realloc(ast, sizeof(node *) * ast_capacity);
    }

    ast[ast_count] = parse_stmt(tokens, &count);
    ast_count++;
  }

  ast[ast_count] = NULL;

  return ast;
}

char *read_file_as_string(const char *filename) {
  FILE *file = fopen(filename, "r");
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = (char *)malloc(file_size + 1);
  size_t bytes_read = fread(buffer, 1, file_size, file);
  buffer[file_size] = '\0';

  fclose(file);
  return buffer;
}

#include <fstream>
#include <string>

void code_gen_body(node **ast, std::string *out);
void code_gen(node *node_value, std::string *out) {
  switch (node_value->node_kind) {
  case kind::set: {
    (*out) +=
        "c = &" + std::string(1, node_value->set_value.register_value[0]) + ";";
    return;
  };
  case kind::add: {
    (*out) += "(*c)++;";
    return;
  };
  case kind::sub: {
    (*out) += "(*c)--;";
    return;
  };
  case kind::loop: {
    (*out) += "while (*c != 0) {\n";
    code_gen_body(node_value->loop_value.body, out);
    (*out) += "\n}\n";
    return;
  };
  case kind::print: {
    (*out) += "o = realloc(o, sizeof(char) * (os + 2));";
    (*out) += "o[os] = (char)(*c); o[os+1] = '\\0';";
    (*out) += "os++;";
    return;
  };
  case kind::input: {
    (*out) += "scanf(\" %c\", &(*c));";
    return;
  };
  case kind::shift_front: {
    (*out) += "h1 = m; h2 = d; h3 = s;";
    (*out) += "m = h3; d = h1; s = h2;";
    return;
  }
  case kind::shift_back: {
    (*out) += "h1 = m; h2 = d; h3 = s;";
    (*out) += "m = h2; d = h3; s = h1;";
    return;
  };
  default:
    funny_error();
    return;
  }
}

void code_gen_body(node **ast, std::string *out) {
  node **ptr = ast;

  while (*ptr != NULL) {
    code_gen(*ptr, out);
    ptr++;
  }
}

int main(int argc, char **argv) {
  char *content = read_file_as_string(argv[1]);
  std::string aot =
      "#include <stdio.h>\n#include <stdlib.h>\n#include "
      "<stdint.h>\n#include <sys/types.h>\nstatic uint8_t "
      "m = 0; "
      "static uint8_t d = 0; static uint8_t s = 0; static uint8_t* c = "
      "&m;\nstatic char* "
      "o; static uint64_t os = 0;\nint main() {\no = malloc(sizeof(char) * "
      "2);\nstatic uint8_t h1 = 0; static uint8_t h2 = 0; static uint8_t h3 = "
      "0;\n";

  code_gen_body(parse(content), &aot);
  aot += "\nprintf(\"%s\\n\", o);\n}\n";
  std::ofstream file(argv[2]);

  if (!file) {
    return 1;
  }

  file << aot;
  file.close();
  return 0;
}
