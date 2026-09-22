#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void limparBuffer(){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

struct paciente{
    int codigo;
    char nome[100];
    int idade;
    char telefone[20];
};

struct medico{
    int codigo;
    char nome[100];
    char especialidade[100];
};

struct consulta{
    int codigoPaciente; // Código do paciente vinculado
    int codigoMedico;   // Código do médico vinculado
    char data[11];
    char horario[6];
    int ativa;          // 1 para ativa, 0 para cancelada
};

// Variáveis globais para armazenar os dados do sistema
struct paciente pacientes[100];
int totalPacientes = 0;

struct medico medicos[50];
int totalMedicos = 0;

struct consulta consultas[200];
int totalConsultas = 0;


/* =======================================================
   PARTE DESENVOLVIDA POR: Pedro Henrique
   Função simples para realizar o cadastro de pacientes
   ======================================================= */
void cadastrarPaciente(){
    if (totalPacientes >= 100) {
        printf("\nErro: Limite de pacientes atingido!\n\n");
        return;
    }

    printf("\n--- CADASTRO DE PACIENTE ---\n");

    printf("Digite o codigo do paciente: ");
    scanf("%d", &pacientes[totalPacientes].codigo);
    limparBuffer();

    printf("Digite o nome do paciente: ");
    fgets(pacientes[totalPacientes].nome, 100, stdin);
    // Remove a quebra de linha do final do nome
    pacientes[totalPacientes].nome[strcspn(pacientes[totalPacientes].nome, "\n")] = '\0';

    printf("Digite a idade do paciente: ");
    scanf("%d", &pacientes[totalPacientes].idade);
    limparBuffer();

    printf("Digite o telefone do paciente: ");
    fgets(pacientes[totalPacientes].telefone, 20, stdin);
    // Remove a quebra de linha do final do telefone
    pacientes[totalPacientes].telefone[strcspn(pacientes[totalPacientes].telefone, "\n")] = '\0';

    totalPacientes++;

    printf("\nPaciente cadastrado com sucesso!\n\n");
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Soldhallas de Melo
   Funções auxiliares de busca utilizando ponteiros
   ======================================================= */
struct paciente* buscarPacientePorCodigo(int codigo) {
    for (int i = 0; i < totalPacientes; i++) {
        if (pacientes[i].codigo == codigo) {
            return &pacientes[i]; // Retorna ponteiro para o paciente
        }
    }
    return NULL;
}

struct medico* buscarMedicoPorCodigo(int codigo) {
    for (int i = 0; i < totalMedicos; i++) {
        if (medicos[i].codigo == codigo) {
            return &medicos[i]; // Retorna ponteiro para o médico
        }
    }
    return NULL;
}


/* =======================================================
   PARTE DESENVOLVIDA POR: João Bernardo
   Função para realizar o cadastro de médicos
   ======================================================= */
void cadastrarMedico() {
    if (totalMedicos >= 50) {
        printf("\nErro: Limite de medicos atingido!\n\n");
        return;
    }

    printf("\n--- CADASTRO DE MEDICO ---\n");

    printf("Digite o codigo do medico: ");
    scanf("%d", &medicos[totalMedicos].codigo);
    limparBuffer();

    printf("Digite o nome do medico: ");
    fgets(medicos[totalMedicos].nome, 100, stdin);
    medicos[totalMedicos].nome[strcspn(medicos[totalMedicos].nome, "\n")] = '\0';

    printf("Digite a especialidade do medico: ");
    fgets(medicos[totalMedicos].especialidade, 100, stdin);
    medicos[totalMedicos].especialidade[strcspn(medicos[totalMedicos].especialidade, "\n")] = '\0';

    totalMedicos++;

    printf("\nMedico cadastrado com sucesso!\n\n");
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Renan Pereira Melo
   Função para realizar o agendamento de consultas
   ======================================================= */
void agendarConsulta() {
    if (totalConsultas >= 200) {
        printf("\nErro: Limite de consultas atingido!\n\n");
        return;
    }

    printf("\n--- AGENDAMENTO DE CONSULTA ---\n");

    int codP, codM;
    printf("Digite o codigo do paciente: ");
    scanf("%d", &codP);

    struct paciente *p = buscarPacientePorCodigo(codP);
    if (p == NULL) {
        printf("\nErro: Paciente nao encontrado!\n\n");
        limparBuffer();
        return;
    }

    printf("Digite o codigo do medico: ");
    scanf("%d", &codM);

    struct medico *m = buscarMedicoPorCodigo(codM);
    if (m == NULL) {
        printf("\nErro: Medico nao encontrado!\n\n");
        limparBuffer();
        return;
    }

    limparBuffer();
    consultas[totalConsultas].codigoPaciente = codP;
    consultas[totalConsultas].codigoMedico = codM;

    printf("Digite a data da consulta (DD/MM/AAAA): ");
    fgets(consultas[totalConsultas].data, 11, stdin);
    consultas[totalConsultas].data[strcspn(consultas[totalConsultas].data, "\n")] = '\0';
    limparBuffer();

    printf("Digite o horario da consulta (HH:MM): ");
    fgets(consultas[totalConsultas].horario, 6, stdin);
    consultas[totalConsultas].horario[strcspn(consultas[totalConsultas].horario, "\n")] = '\0';
    limparBuffer();

    consultas[totalConsultas].ativa = 1; // Marca a consulta como ativa
    totalConsultas++;

    printf("\nConsulta agendada com sucesso para o paciente %s com o medico %s!\n\n", p->nome, m->nome);
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Soldhallas de Melo
   Função para cancelar/desativar uma consulta existente
   ======================================================= */
void cancelarConsulta() {
    if (totalConsultas == 0) {
        printf("\nNenhuma consulta cadastrada no sistema!\n\n");
        return;
    }

    printf("\n--- CANCELAR CONSULTA ---\n");
    int codP, codM;
    printf("Digite o codigo do paciente: ");
    scanf("%d", &codP);
    printf("Digite o codigo do medico: ");
    scanf("%d", &codM);
    limparBuffer();

    int achou = 0;
    for (int i = 0; i < totalConsultas; i++) {
        if (consultas[i].codigoPaciente == codP && consultas[i].codigoMedico == codM && consultas[i].ativa == 1) {
            consultas[i].ativa = 0; // Altera o status para desativa/cancelada
            achou = 1;
            printf("\nConsulta cancelada com sucesso!\n\n");
            break;
        }
    }

    if (!achou) {
        printf("\nNenhuma consulta ativa encontrada para esses codigos!\n\n");
    }
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Pedro Henrique e João Bernardo
   Função para realizar a busca de pacientes por código ou nome
   ======================================================= */
void buscarPaciente() {
    if (totalPacientes == 0) {
        printf("\nNenhum paciente cadastrado!\n\n");
        return;
    }

    printf("\n--- BUSCA DE PACIENTE ---\n");
    printf("1. Buscar por codigo\n");
    printf("2. Buscar por nome\n");
    printf("Opcao: ");
    int op;
    scanf("%d", &op);
    limparBuffer();

    if (op == 1) {
        int cod;
        printf("Digite o codigo do paciente: ");
        scanf("%d", &cod);
        limparBuffer();

        struct paciente *p = buscarPacientePorCodigo(cod);
        if (p != NULL) {
            printf("\n--- Paciente Encontrado ---\n");
            printf("Codigo: %d\n", p->codigo);
            printf("Nome: %s\n", p->nome);
            printf("Idade: %d\n", p->idade);
            printf("Telefone: %s\n\n", p->telefone);
        } else {
            printf("\nPaciente nao encontrado!\n\n");
        }
    } else if (op == 2) {
        char nomeBusca[100];
        printf("Digite o nome (ou parte do nome): ");
        fgets(nomeBusca, 100, stdin);
        nomeBusca[strcspn(nomeBusca, "\n")] = '\0';

        int achou = 0;
        for (int i = 0; i < totalPacientes; i++) {
            if (strstr(pacientes[i].nome, nomeBusca) != NULL) {
                printf("\n--- Paciente Encontrado ---\n");
                printf("Codigo: %d\n", pacientes[i].codigo);
                printf("Nome: %s\n", pacientes[i].nome);
                printf("Idade: %d\n", pacientes[i].idade);
                printf("Telefone: %s\n\n", pacientes[i].telefone);
                achou = 1;
            }
        }
        if (!achou) {
            printf("\nNenhum paciente encontrado com esse nome!\n\n");
        }
    } else {
        printf("\nOpcao invalida!\n\n");
    }
}


/* =======================================================
   PARTE DESENVOLVIDA POR: Lohan Fontenele Brito
   Função para exibir relatório geral do sistema e listagens
   ======================================================= */
void relatorioGeral() {
    printf("\n================ RELATORIO GERAL ================\n");
    printf("Total de Pacientes Cadastrados: %d\n", totalPacientes);
    printf("Total de Medicos Cadastrados:   %d\n", totalMedicos);
    printf("Total de Consultas Registradas: %d\n", totalConsultas);
    printf("=================================================\n\n");

    printf("--- LISTA DE PACIENTES ---\n");
    if (totalPacientes == 0) {
        printf("Nenhum paciente cadastrado.\n");
    } else {
        for (int i = 0; i < totalPacientes; i++) {
            printf("[%d] Codigo: %d | Nome: %s | Idade: %d | Tel: %s\n", 
                   i+1, pacientes[i].codigo, pacientes[i].nome, pacientes[i].idade, pacientes[i].telefone);
        }
    }

    printf("\n--- LISTA DE MEDICOS ---\n");
    if (totalMedicos == 0) {
        printf("Nenhum medico cadastrado.\n");
    } else {
        for (int i = 0; i < totalMedicos; i++) {
            printf("[%d] Codigo: %d | Nome: %s | Especialidade: %s\n", 
                   i+1, medicos[i].codigo, medicos[i].nome, medicos[i].especialidade);
        }
    }

    printf("\n--- LISTA DE CONSULTAS ---\n");
    if (totalConsultas == 0) {
        printf("Nenhuma consulta cadastrada.\n");
    } else {
        for (int i = 0; i < totalConsultas; i++) {
            struct paciente *p = buscarPacientePorCodigo(consultas[i].codigoPaciente);
            struct medico *m = buscarMedicoPorCodigo(consultas[i].codigoMedico);
            
            char *status = consultas[i].ativa ? "ATIVA" : "CANCELADA";
            
            printf("[%d] Paciente: %s | Medico: %s | Data: %s | Hora: %s | Status: %s\n",
                   i+1, 
                   (p != NULL) ? p->nome : "Desconhecido", 
                   (m != NULL) ? m->nome : "Desconhecido", 
                   consultas[i].data, 
                   consultas[i].horario, 
                   status);
        }
    }
    printf("\n");
}


int main(){

    printf("--------- GRUPO 4 - SISTEMA DE CLÍNICA -----------     \n");
    printf("---- Renan Pereira Melo                     🔴 🟩 🟩   \n");
    printf("---- Soldhallas de Melo Fontenele           🟩 🟩      \n");
    printf("---- Lohan Fontenele Brito                  🟩 🟩 🟩   \n");
    printf("---- Pedro Henrique Sousa da Silva          🟩 🟩      \n");
    printf("---- João Bernardo                    Instituto Federal\n");
    printf("                                       Campus Tianguá  \n");
    
    int menu = -1;
    int validarScanf = 0;
    while(menu != 0){
        printf("       MENU SISTEMA DE CLÍNICA        \n");
        printf("======================================\n");
        printf("Cadastrar paciente: [1]               \n");
        printf("Cadastrar médico:   [2]               \n");
        printf("Agendar consulta:   [3]               \n");
        printf("Cancelar consulta:  [4]               \n");
        printf("Busca de paciente:  [5]               \n");
        printf("Relatorio geral:    [6]               \n");
        printf("Sair do sistema:    [0]               \n");
        printf("======================================\n");
        
        printf("\nOpção: [_]\b\b");
        validarScanf = scanf("%d", &menu);
        if(validarScanf != 1){
            limparBuffer();
            printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        }
        switch(menu){
            
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

            case 0:
                printf("\nSaindo do sistema...\n");
                break;
                
            default:
                printf("\nOpção inválida!\n\n");
                break;
        }
       
    }

    return 0;
}
