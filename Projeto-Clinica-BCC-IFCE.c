#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* ======================= CONSTANTES ======================= */
#define MAX_PACIENTES  100
#define MAX_MEDICOS     50
#define MAX_CONSULTAS  200

#define TAM_NOME       100
#define TAM_TELEFONE    20
#define TAM_DATA        11   /* DD/MM/AAAA + '\0' */
#define TAM_HORARIO      6   /* HH:MM + '\0'      */

#define ARQUIVO_DADOS  "clinica.dat"

/* ======================= STRUCTS ======================= */
struct paciente {
    int  codigo;
    char nome[TAM_NOME];
    int  idade;
    char telefone[TAM_TELEFONE];
};

struct medico {
    int  codigo;
    char nome[TAM_NOME];
    char especialidade[TAM_NOME];
};

struct consulta {
    int  codigoPaciente;      /* Código do paciente vinculado */
    int  codigoMedico;        /* Código do médico vinculado   */
    char data[TAM_DATA];
    char horario[TAM_HORARIO];
    int  ativa;               /* 1 = ativa, 0 = cancelada     */
};

/* ======================= DADOS GLOBAIS ======================= */
struct paciente pacientes[MAX_PACIENTES];
int totalPacientes = 0;

struct medico medicos[MAX_MEDICOS];
int totalMedicos = 0;

struct consulta consultas[MAX_CONSULTAS];
int totalConsultas = 0;


/* =======================================================
   ENTRADA SEGURA E UTILITÁRIOS
   Toda leitura é feita por linha (fgets), então o buffer
   nunca fica com "sobras" entre um campo e outro.
   ======================================================= */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Se a entrada acabar (Ctrl+D / Ctrl+Z), encerra sem ficar em loop infinito */
void encerrarPorEOF(void);

/* Remove espaços do início e do fim da string */
void aparar(char *s) {
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[--len] = '\0';
    }
    size_t ini = 0;
    while (s[ini] && isspace((unsigned char)s[ini])) ini++;
    if (ini > 0) memmove(s, s + ini, strlen(s + ini) + 1);
}

/* Lê uma linha de texto não vazia. Rejeita entradas maiores que o campo.
   Lê primeiro num buffer grande (para o '\n' não contar como "sobra") e
   só depois confere se cabe em dest. */
void lerString(const char *prompt, char *dest, int tam) {
    char linha[256];
    while (1) {
        printf("%s", prompt);
        if (fgets(linha, sizeof linha, stdin) == NULL) encerrarPorEOF();

        char *nl = strchr(linha, '\n');
        if (nl) {
            *nl = '\0';
        } else if (!feof(stdin)) {
            /* Linha maior que o buffer: descarta o resto */
            limparBuffer();
            printf("Texto muito longo (maximo %d caracteres). Tente novamente.\n", tam - 1);
            continue;
        }

        aparar(linha);
        if (linha[0] == '\0') {
            printf("Este campo nao pode ficar vazio.\n");
            continue;
        }
        if ((int)strlen(linha) >= tam) {
            printf("Texto muito longo (maximo %d caracteres). Tente novamente.\n", tam - 1);
            continue;
        }

        strcpy(dest, linha);
        return;
    }
}

/* Lê um inteiro dentro de [min, max], repetindo até ser válido */
int lerInt(const char *prompt, int min, int max) {
    char buf[32];
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof buf, stdin) == NULL) encerrarPorEOF();

        if (!strchr(buf, '\n') && !feof(stdin)) {
            limparBuffer();
            printf("Entrada invalida. Digite um numero entre %d e %d.\n", min, max);
            continue;
        }

        char *fim;
        errno = 0;
        long v = strtol(buf, &fim, 10);
        if (fim == buf) {
            printf("Entrada invalida. Digite um numero entre %d e %d.\n", min, max);
            continue;
        }
        while (isspace((unsigned char)*fim)) fim++;
        if (*fim != '\0' || errno == ERANGE || v < min || v > max) {
            printf("Entrada invalida. Digite um numero entre %d e %d.\n", min, max);
            continue;
        }
        return (int)v;
    }
}

/* Valida DD/MM/AAAA, incluindo dias de cada mês e ano bissexto */
int dataValida(const char *s) {
    if (strlen(s) != 10 || s[2] != '/' || s[5] != '/') return 0;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit((unsigned char)s[i])) return 0;
    }

    int d, m, a;
    if (sscanf(s, "%d/%d/%d", &d, &m, &a) != 3) return 0;
    if (a < 2000 || a > 2100 || m < 1 || m > 12 || d < 1) return 0;

    int dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int bissexto = (a % 4 == 0 && a % 100 != 0) || (a % 400 == 0);
    if (m == 2 && bissexto) dias[1] = 29;

    return d <= dias[m - 1];
}

/* Valida HH:MM (24h, com zero à esquerda: 09:30) */
int horarioValido(const char *s) {
    if (strlen(s) != 5 || s[2] != ':') return 0;
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    int h, m;
    if (sscanf(s, "%d:%d", &h, &m) != 2) return 0;
    return h >= 0 && h <= 23 && m >= 0 && m <= 59;
}

/* Telefone: apenas dígitos, espaço, parênteses, hífen e '+', com ao menos 8 dígitos */
int telefoneValido(const char *s) {
    int digitos = 0;
    for (; *s; s++) {
        if (isdigit((unsigned char)*s)) digitos++;
        else if (*s != ' ' && *s != '(' && *s != ')' && *s != '-' && *s != '+') return 0;
    }
    return digitos >= 8;
}

void lerData(char *dest) {
    while (1) {
        lerString("Digite a data da consulta (DD/MM/AAAA): ", dest, TAM_DATA);
        if (dataValida(dest)) return;
        printf("Data invalida. Use o formato DD/MM/AAAA com uma data real.\n");
    }
}

void lerHorario(char *dest) {
    while (1) {
        lerString("Digite o horario da consulta (HH:MM): ", dest, TAM_HORARIO);
        if (horarioValido(dest)) return;
        printf("Horario invalido. Use o formato HH:MM (ex.: 09:30).\n");
    }
}

/* Comparação de texto ignorando maiúsculas/minúsculas (strstr não faz isso) */
int contemIgnorandoCaso(const char *texto, const char *trecho) {
    size_t n = strlen(trecho);
    if (n == 0) return 1;
    for (; *texto; texto++) {
        size_t i = 0;
        while (i < n && texto[i] &&
               tolower((unsigned char)texto[i]) == tolower((unsigned char)trecho[i])) {
            i++;
        }
        if (i == n) return 1;
    }
    return 0;
}

void limparTela(void) {
#ifdef _WIN32
    if (system("cls")) { }
#else
    if (system("clear")) { }
#endif
}

void pausar(void) {
    printf("Pressione ENTER para continuar...");
    limparBuffer();
}


/* =======================================================
   PERSISTÊNCIA EM ARQUIVO (binário)
   Formato: [totalPacientes][pacientes...]
            [totalMedicos][medicos...]
            [totalConsultas][consultas...]
   ======================================================= */
int salvarDados(void) {
    FILE *f = fopen(ARQUIVO_DADOS, "wb");
    if (f == NULL) {
        printf("Aviso: nao foi possivel salvar os dados em '%s'.\n", ARQUIVO_DADOS);
        return 0;
    }

    int ok =
        fwrite(&totalPacientes, sizeof(int), 1, f) == 1 &&
        fwrite(pacientes, sizeof(struct paciente), totalPacientes, f) == (size_t)totalPacientes &&
        fwrite(&totalMedicos, sizeof(int), 1, f) == 1 &&
        fwrite(medicos, sizeof(struct medico), totalMedicos, f) == (size_t)totalMedicos &&
        fwrite(&totalConsultas, sizeof(int), 1, f) == 1 &&
        fwrite(consultas, sizeof(struct consulta), totalConsultas, f) == (size_t)totalConsultas;

    fclose(f);
    if (!ok) printf("Aviso: erro ao gravar os dados no arquivo.\n");
    return ok;
}

void carregarDados(void) {
    FILE *f = fopen(ARQUIVO_DADOS, "rb");
    if (f == NULL) return;  /* primeira execução: ainda não há arquivo */

    int tp = 0, tm = 0, tc = 0;
    int ok = 1;

    ok = ok && fread(&tp, sizeof(int), 1, f) == 1 && tp >= 0 && tp <= MAX_PACIENTES &&
         fread(pacientes, sizeof(struct paciente), tp, f) == (size_t)tp;
    ok = ok && fread(&tm, sizeof(int), 1, f) == 1 && tm >= 0 && tm <= MAX_MEDICOS &&
         fread(medicos, sizeof(struct medico), tm, f) == (size_t)tm;
    ok = ok && fread(&tc, sizeof(int), 1, f) == 1 && tc >= 0 && tc <= MAX_CONSULTAS &&
         fread(consultas, sizeof(struct consulta), tc, f) == (size_t)tc;

    fclose(f);

    if (ok) {
        totalPacientes = tp;
        totalMedicos = tm;
        totalConsultas = tc;
        printf("Dados carregados: %d paciente(s), %d medico(s), %d consulta(s).\n",
               totalPacientes, totalMedicos, totalConsultas);
    } else {
        totalPacientes = totalMedicos = totalConsultas = 0;
        printf("Aviso: arquivo de dados corrompido. Iniciando com o sistema vazio.\n");
    }
}

void encerrarPorEOF(void) {
    printf("\nEntrada encerrada. Salvando e saindo...\n");
    salvarDados();
    exit(0);
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Soldhallas de Melo
   Funções auxiliares de busca utilizando ponteiros
   ======================================================= */
struct paciente* buscarPacientePorCodigo(int codigo) {
    for (int i = 0; i < totalPacientes; i++) {
        if (pacientes[i].codigo == codigo) {
            return &pacientes[i]; /* Retorna ponteiro para o paciente */
        }
    }
    return NULL;
}

struct medico* buscarMedicoPorCodigo(int codigo) {
    for (int i = 0; i < totalMedicos; i++) {
        if (medicos[i].codigo == codigo) {
            return &medicos[i]; /* Retorna ponteiro para o médico */
        }
    }
    return NULL;
}

/* Verifica se o médico ou o paciente já têm consulta ativa nesse dia e horário.
   Retorna 1 (médico ocupado), 2 (paciente ocupado) ou 0 (livre). */
int existeConflito(int codP, int codM, const char *data, const char *horario) {
    for (int i = 0; i < totalConsultas; i++) {
        if (!consultas[i].ativa) continue;
        if (strcmp(consultas[i].data, data) != 0) continue;
        if (strcmp(consultas[i].horario, horario) != 0) continue;

        if (consultas[i].codigoMedico == codM) return 1;
        if (consultas[i].codigoPaciente == codP) return 2;
    }
    return 0;
}

void imprimirPaciente(const struct paciente *p) {
    printf("Codigo: %d\n", p->codigo);
    printf("Nome: %s\n", p->nome);
    printf("Idade: %d\n", p->idade);
    printf("Telefone: %s\n\n", p->telefone);
}

void imprimirConsulta(int i) {
    struct paciente *p = buscarPacientePorCodigo(consultas[i].codigoPaciente);
    struct medico *m = buscarMedicoPorCodigo(consultas[i].codigoMedico);

    printf("[%d] Paciente: %s | Medico: %s | Data: %s | Hora: %s | Status: %s\n",
           i + 1,
           (p != NULL) ? p->nome : "Desconhecido",
           (m != NULL) ? m->nome : "Desconhecido",
           consultas[i].data,
           consultas[i].horario,
           consultas[i].ativa ? "ATIVA" : "CANCELADA");
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Pedro Henrique
   Função para realizar o cadastro de pacientes
   ======================================================= */
void cadastrarPaciente(void) {
    if (totalPacientes >= MAX_PACIENTES) {
        printf("\nErro: Limite de pacientes atingido!\n\n");
        return;
    }

    struct paciente novo;
    printf("\n--- CADASTRO DE PACIENTE ---\n");

    while (1) {
        novo.codigo = lerInt("Digite o codigo do paciente: ", 1, 999999);
        if (buscarPacientePorCodigo(novo.codigo) == NULL) break;
        printf("Erro: ja existe um paciente com o codigo %d. Escolha outro.\n", novo.codigo);
    }

    lerString("Digite o nome do paciente: ", novo.nome, TAM_NOME);
    novo.idade = lerInt("Digite a idade do paciente: ", 0, 120);

    while (1) {
        lerString("Digite o telefone do paciente: ", novo.telefone, TAM_TELEFONE);
        if (telefoneValido(novo.telefone)) break;
        printf("Telefone invalido. Use apenas numeros (minimo 8 digitos), espacos, ( ) - e +.\n");
    }

    pacientes[totalPacientes++] = novo;
    salvarDados();

    printf("\nPaciente cadastrado com sucesso!\n\n");
}


/* =======================================================
   PARTE DESENVOLVIDA POR: João Bernardo
   Função para realizar o cadastro de médicos
   ======================================================= */
void cadastrarMedico(void) {
    if (totalMedicos >= MAX_MEDICOS) {
        printf("\nErro: Limite de medicos atingido!\n\n");
        return;
    }

    struct medico novo;
    printf("\n--- CADASTRO DE MEDICO ---\n");

    while (1) {
        novo.codigo = lerInt("Digite o codigo do medico: ", 1, 999999);
        if (buscarMedicoPorCodigo(novo.codigo) == NULL) break;
        printf("Erro: ja existe um medico com o codigo %d. Escolha outro.\n", novo.codigo);
    }

    lerString("Digite o nome do medico: ", novo.nome, TAM_NOME);
    lerString("Digite a especialidade do medico: ", novo.especialidade, TAM_NOME);

    medicos[totalMedicos++] = novo;
    salvarDados();

    printf("\nMedico cadastrado com sucesso!\n\n");
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Renan Pereira Melo
   Função para realizar o agendamento de consultas
   ======================================================= */
void agendarConsulta(void) {
    if (totalConsultas >= MAX_CONSULTAS) {
        printf("\nErro: Limite de consultas atingido!\n\n");
        return;
    }
    if (totalPacientes == 0 || totalMedicos == 0) {
        printf("\nErro: cadastre ao menos um paciente e um medico antes de agendar.\n\n");
        return;
    }

    printf("\n--- AGENDAMENTO DE CONSULTA ---\n");

    int codP = lerInt("Digite o codigo do paciente: ", 1, 999999);
    struct paciente *p = buscarPacientePorCodigo(codP);
    if (p == NULL) {
        printf("\nErro: Paciente nao encontrado!\n\n");
        return;
    }

    int codM = lerInt("Digite o codigo do medico: ", 1, 999999);
    struct medico *m = buscarMedicoPorCodigo(codM);
    if (m == NULL) {
        printf("\nErro: Medico nao encontrado!\n\n");
        return;
    }

    struct consulta nova;
    nova.codigoPaciente = codP;
    nova.codigoMedico = codM;
    lerData(nova.data);
    lerHorario(nova.horario);

    int conflito = existeConflito(codP, codM, nova.data, nova.horario);
    if (conflito == 1) {
        printf("\nErro: o medico %s ja tem uma consulta ativa em %s as %s.\n\n",
               m->nome, nova.data, nova.horario);
        return;
    }
    if (conflito == 2) {
        printf("\nErro: o paciente %s ja tem uma consulta ativa em %s as %s.\n\n",
               p->nome, nova.data, nova.horario);
        return;
    }

    nova.ativa = 1; /* Marca a consulta como ativa */
    consultas[totalConsultas++] = nova;
    salvarDados();

    printf("\nConsulta agendada com sucesso para o paciente %s com o medico %s!\n\n",
           p->nome, m->nome);
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Soldhallas de Melo
   Função para cancelar/desativar uma consulta existente
   (agora identifica a consulta por paciente, médico, data e horário)
   ======================================================= */
void cancelarConsulta(void) {
    if (totalConsultas == 0) {
        printf("\nNenhuma consulta cadastrada no sistema!\n\n");
        return;
    }

    printf("\n--- CANCELAR CONSULTA ---\n");
    int codP = lerInt("Digite o codigo do paciente: ", 1, 999999);
    int codM = lerInt("Digite o codigo do medico: ", 1, 999999);

    /* Mostra as consultas ativas dessa dupla para o usuário escolher */
    int qtd = 0;
    for (int i = 0; i < totalConsultas; i++) {
        if (consultas[i].codigoPaciente == codP &&
            consultas[i].codigoMedico == codM &&
            consultas[i].ativa) {
            if (qtd == 0) printf("\nConsultas ativas encontradas:\n");
            imprimirConsulta(i);
            qtd++;
        }
    }

    if (qtd == 0) {
        printf("\nNenhuma consulta ativa encontrada para esses codigos!\n\n");
        return;
    }

    char data[TAM_DATA], horario[TAM_HORARIO];
    printf("\nInforme qual consulta cancelar:\n");
    lerData(data);
    lerHorario(horario);

    for (int i = 0; i < totalConsultas; i++) {
        if (consultas[i].codigoPaciente == codP &&
            consultas[i].codigoMedico == codM &&
            consultas[i].ativa &&
            strcmp(consultas[i].data, data) == 0 &&
            strcmp(consultas[i].horario, horario) == 0) {
            consultas[i].ativa = 0; /* Altera o status para cancelada */
            salvarDados();
            printf("\nConsulta cancelada com sucesso!\n\n");
            return;
        }
    }

    printf("\nNenhuma consulta ativa nessa data e horario!\n\n");
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Pedro Henrique e João Bernardo
   Função para realizar a busca de pacientes por código ou nome
   (busca por nome agora ignora maiúsculas/minúsculas)
   ======================================================= */
void buscarPaciente(void) {
    if (totalPacientes == 0) {
        printf("\nNenhum paciente cadastrado!\n\n");
        return;
    }

    printf("\n--- BUSCA DE PACIENTE ---\n");
    printf("1. Buscar por codigo\n");
    printf("2. Buscar por nome\n");
    int op = lerInt("Opcao: ", 1, 2);

    if (op == 1) {
        int cod = lerInt("Digite o codigo do paciente: ", 1, 999999);

        struct paciente *p = buscarPacientePorCodigo(cod);
        if (p != NULL) {
            printf("\n--- Paciente Encontrado ---\n");
            imprimirPaciente(p);
        } else {
            printf("\nPaciente nao encontrado!\n\n");
        }
    } else {
        char nomeBusca[TAM_NOME];
        lerString("Digite o nome (ou parte do nome): ", nomeBusca, TAM_NOME);

        int achou = 0;
        for (int i = 0; i < totalPacientes; i++) {
            if (contemIgnorandoCaso(pacientes[i].nome, nomeBusca)) {
                printf("\n--- Paciente Encontrado ---\n");
                imprimirPaciente(&pacientes[i]);
                achou = 1;
            }
        }
        if (!achou) {
            printf("\nNenhum paciente encontrado com esse nome!\n\n");
        }
    }
}


/* =======================================================
   NOVO: listagem de consultas de um paciente ou de um médico
   ======================================================= */
void listarConsultasPorPessoa(void) {
    if (totalConsultas == 0) {
        printf("\nNenhuma consulta cadastrada no sistema!\n\n");
        return;
    }

    printf("\n--- LISTAR CONSULTAS ---\n");
    printf("1. De um paciente\n");
    printf("2. De um medico\n");
    int op = lerInt("Opcao: ", 1, 2);
    int cod = lerInt("Digite o codigo: ", 1, 999999);

    if (op == 1 && buscarPacientePorCodigo(cod) == NULL) {
        printf("\nErro: Paciente nao encontrado!\n\n");
        return;
    }
    if (op == 2 && buscarMedicoPorCodigo(cod) == NULL) {
        printf("\nErro: Medico nao encontrado!\n\n");
        return;
    }

    int achou = 0;
    printf("\n");
    for (int i = 0; i < totalConsultas; i++) {
        int pertence = (op == 1) ? (consultas[i].codigoPaciente == cod)
                                 : (consultas[i].codigoMedico == cod);
        if (pertence) {
            imprimirConsulta(i);
            achou = 1;
        }
    }
    if (!achou) printf("Nenhuma consulta encontrada.\n");
    printf("\n");
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Lohan Fontenele Brito
   Função para exibir relatório geral do sistema e listagens
   ======================================================= */
void relatorioGeral(void) {
    int ativas = 0;
    for (int i = 0; i < totalConsultas; i++) {
        if (consultas[i].ativa) ativas++;
    }

    printf("\n================ RELATORIO GERAL ================\n");
    printf("Total de Pacientes Cadastrados: %d\n", totalPacientes);
    printf("Total de Medicos Cadastrados:   %d\n", totalMedicos);
    printf("Total de Consultas Registradas: %d (%d ativas, %d canceladas)\n",
           totalConsultas, ativas, totalConsultas - ativas);
    printf("=================================================\n\n");

    printf("--- LISTA DE PACIENTES ---\n");
    if (totalPacientes == 0) {
        printf("Nenhum paciente cadastrado.\n");
    } else {
        for (int i = 0; i < totalPacientes; i++) {
            printf("[%d] Codigo: %d | Nome: %s | Idade: %d | Tel: %s\n",
                   i + 1, pacientes[i].codigo, pacientes[i].nome,
                   pacientes[i].idade, pacientes[i].telefone);
        }
    }

    printf("\n--- LISTA DE MEDICOS ---\n");
    if (totalMedicos == 0) {
        printf("Nenhum medico cadastrado.\n");
    } else {
        for (int i = 0; i < totalMedicos; i++) {
            printf("[%d] Codigo: %d | Nome: %s | Especialidade: %s\n",
                   i + 1, medicos[i].codigo, medicos[i].nome, medicos[i].especialidade);
        }
    }

    printf("\n--- LISTA DE CONSULTAS ---\n");
    if (totalConsultas == 0) {
        printf("Nenhuma consulta cadastrada.\n");
    } else {
        for (int i = 0; i < totalConsultas; i++) {
            imprimirConsulta(i);
        }
    }
    printf("\n");
}


int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  /* acentos e emojis no terminal do Windows */
#endif

    printf("--------- GRUPO 4 - SISTEMA DE CLÍNICA -----------     \n");
    printf("---- Renan Pereira Melo                     🔴 🟩 🟩   \n");
    printf("---- Soldhallas de Melo Fontenele           🟩 🟩      \n");
    printf("---- Lohan Fontenele Brito                  🟩 🟩 🟩   \n");
    printf("---- Pedro Henrique Sousa da Silva          🟩 🟩      \n");
    printf("---- João Bernardo                    Instituto Federal\n");
    printf("                                       Campus Tianguá  \n\n");

    carregarDados();
    pausar();

    int menu = -1;
    while (menu != 0) {
        limparTela();
        printf("       MENU SISTEMA DE CLÍNICA        \n");
        printf("======================================\n");
        printf("Cadastrar paciente:        [1]        \n");
        printf("Cadastrar médico:          [2]        \n");
        printf("Agendar consulta:          [3]        \n");
        printf("Cancelar consulta:         [4]        \n");
        printf("Busca de paciente:         [5]        \n");
        printf("Relatorio geral:           [6]        \n");
        printf("Consultas de paciente/med: [7]        \n");
        printf("Sair do sistema:           [0]        \n");
        printf("======================================\n\n");

        menu = lerInt("Opção: ", 0, 7);

        switch (menu) {
            case 1:
                /* Parte desenvolvida por: Pedro Henrique */
                cadastrarPaciente();
                break;

            case 2:
                /* Parte desenvolvida por: João Bernardo */
                cadastrarMedico();
                break;

            case 3:
                /* Parte desenvolvida por: Renan Pereira Melo */
                agendarConsulta();
                break;

            case 4:
                /* Parte desenvolvida por: Soldhallas de Melo */
                cancelarConsulta();
                break;

            case 5:
                /* Parte desenvolvida por: Pedro Henrique e João Bernardo */
                buscarPaciente();
                break;

            case 6:
                /* Parte desenvolvida por: Lohan Fontenele Brito */
                relatorioGeral();
                break;

            case 7:
                listarConsultasPorPessoa();
                break;

            case 0:
                salvarDados();
                printf("\nSaindo do sistema...\n");
                break;
        }

        if (menu != 0) pausar();
    }

    return 0;
}