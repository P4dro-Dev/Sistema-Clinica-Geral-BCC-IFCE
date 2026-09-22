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
    int codigoMedico;
    char data[11];
    char horario[6];
    int ativa;
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
                /
                cadastrarPaciente();
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