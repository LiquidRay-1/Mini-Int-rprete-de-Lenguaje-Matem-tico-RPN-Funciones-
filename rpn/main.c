#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// -------------------------
// Estructuras básicas
// -------------------------
typedef struct Node
{
    int is_operator;
    char op;
    int is_variable;
    char var_name[32];
    int value;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct Var
{
    char name[32];
    int value;
    struct Var *next;
} Var;

typedef struct Func
{
    char name[32];
    char params[4][32];
    int param_count;
    char *body;
    struct Func *next;
} Func;

Var *var_table = NULL;
Func *func_table = NULL;

// -------------------------
// Gestión de variables
// -------------------------
void set_var(const char *name, int value)
{
    Var *v = var_table;
    while (v)
    {
        if (strcmp(v->name, name) == 0)
        {
            v->value = value;
            return;
        }
        v = v->next;
    }
    Var *new_var = malloc(sizeof(Var));
    strcpy(new_var->name, name);
    new_var->value = value;
    new_var->next = var_table;
    var_table = new_var;
}

int get_var(const char *name)
{
    Var *v = var_table;
    while (v)
    {
        if (strcmp(v->name, name) == 0)
            return v->value;
        v = v->next;
    }
    printf("Error: variable '%s' no definida.\n", name);
    exit(1);
}

// -------------------------
// Gestión de funciones
// -------------------------
void add_func(const char *name, char params[][32], int count, const char *body)
{
    Func *f = malloc(sizeof(Func));
    strcpy(f->name, name);
    f->param_count = count;
    for (int i = 0; i < count; i++)
        strcpy(f->params[i], params[i]);
    f->body = strdup(body);
    f->next = func_table;
    func_table = f;
}

Func *get_func(const char *name)
{
    Func *f = func_table;
    while (f)
    {
        if (strcmp(f->name, name) == 0)
            return f;
        f = f->next;
    }
    return NULL;
}

// -------------------------
// Crear nodos
// -------------------------
Node *create_number_node(int value)
{
    Node *n = malloc(sizeof(Node));
    n->is_operator = 0;
    n->is_variable = 0;
    n->value = value;
    n->left = n->right = NULL;
    return n;
}

Node *create_var_node(const char *name)
{
    Node *n = malloc(sizeof(Node));
    n->is_operator = 0;
    n->is_variable = 1;
    strcpy(n->var_name, name);
    n->left = n->right = NULL;
    return n;
}

Node *create_operator_node(char op, Node *left, Node *right)
{
    Node *n = malloc(sizeof(Node));
    n->is_operator = 1;
    n->op = op;
    n->left = left;
    n->right = right;
    return n;
}

// -------------------------
// str to int (bitwise)
// -------------------------
int str_to_int(const char *str)
{
    int sign = 1;
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    int r = 0;
    while (*str)
        r = (r << 3) + (r << 1) + (*str++ - '0');
    return r * sign;
}

// -------------------------
// Parser RPN → Árbol
// -------------------------
#include <assert.h>

typedef struct
{
    Node *items[64];
    int top;
} Stack;

void push(Stack *s, Node *n) { s->items[++s->top] = n; }
Node *pop(Stack *s) { return s->items[s->top--]; }

Node *parse_rpn(const char *expr)
{
    Stack s = {.top = -1};
    char token[32];
    int i = 0, j = 0;
    while (1)
    {
        if (expr[i] == ' ' || expr[i] == '\0')
        {
            if (j == 0)
            {
                if (expr[i] == '\0')
                    break;
                i++;
                continue;
            }
            token[j] = '\0';
            j = 0;

            if (strlen(token) == 1 && strchr("+-*/", token[0]))
            {
                Node *r = pop(&s), *l = pop(&s);
                push(&s, create_operator_node(token[0], l, r));
            }
            else if (isalpha(token[0]))
            {
                push(&s, create_var_node(token));
            }
            else
            {
                push(&s, create_number_node(str_to_int(token)));
            }
        }
        else
        {
            token[j++] = expr[i];
        }
        i++;
    }
    return pop(&s);
}

// -------------------------
// Evaluar árbol
// -------------------------
int eval(Node *n)
{
    if (!n)
        return 0;
    if (!n->is_operator)
    {
        if (n->is_variable)
            return get_var(n->var_name);
        return n->value;
    }
    int l = eval(n->left);
    int r = eval(n->right);
    switch (n->op)
    {
    case '+':
        return l + r;
    case '-':
        return l - r;
    case '*':
        return l * r;
    case '/':
        return r ? l / r : 0;
    }
    return 0;
}

// -------------------------
// Ejecutar línea
// -------------------------
void execute_line(const char *line)
{
    char name[32], expr[128];

    if (sscanf(line, "%31s = %[^\n]", name, expr) == 2)
    {
        Node *n = parse_rpn(expr);
        int v = eval(n);
        set_var(name, v);
        printf("[set] %s = %d\n", name, v);
        return;
    }

    if (strncmp(line, "def ", 4) == 0)
    {
        char fname[32], param1[32], param2[32], param3[32];
        int offset = 4, param_count = 0;
        sscanf(line + offset, "%31s", fname);
        offset += strlen(fname) + 1;
        const char *p = line + offset;
        char params[4][32];

        while (sscanf(p, "%31s", params[param_count]) == 1 && !isdigit(params[param_count][0]) && strcmp(params[param_count], "end") != 0)
        {
            p += strlen(params[param_count]) + 1;
            param_count++;
        }

        const char *body = strchr(line, params[param_count - 1]) + strlen(params[param_count - 1]) + 1;
        add_func(fname, params, param_count, body);
        printf("[def] función '%s' definida\n", fname);
        return;
    }

    char arg1[32], arg2[32], arg3[32];
    int count = sscanf(line, "%31s %31s %31s", name, arg1, arg2);
    Func *f = get_func(name);
    if (f)
    {
        if ((f->param_count == 1 && count >= 2) || (f->param_count == 2 && count == 3))
        {
            int args[2];
            args[0] = isalpha(arg1[0]) ? get_var(arg1) : str_to_int(arg1);
            args[1] = isalpha(arg2[0]) ? get_var(arg2) : str_to_int(arg2);

            // Guardar vars antiguas
            Var *backup = var_table;
            var_table = NULL;

            for (int i = 0; i < f->param_count; i++)
                set_var(f->params[i], args[i]);

            Node *tree = parse_rpn(f->body);
            int res = eval(tree);
            printf("[call] %s = %d\n", name, res);

            // Restaurar entorno
            while (var_table)
            {
                Var *temp = var_table;
                var_table = var_table->next;
                free(temp);
            }
            var_table = backup;
            return;
        }
    }

    // Último recurso: evaluar como RPN
    Node *t = parse_rpn(line);
    int res = eval(t);
    printf("[eval] %d\n", res);
}

// -------------------------
// Main
// -------------------------
int main()
{
    char line[256];

    printf("MiniIntérprete Matemático\n");
    printf("Escribe expresiones RPN, asignaciones, o define funciones.\n");
    printf("Ejemplo:\n");
    printf("  x = 3 4 +\n");
    printf("  def cuadrado a a a * end\n");
    printf("  cuadrado x\n");

    while (1)
    {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin))
            break;
        if (strncmp(line, "exit", 4) == 0)
            break;
        execute_line(line);
    }

    return 0;
}
