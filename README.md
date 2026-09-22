# 🏥 Sistema de Gestão de Clínica Médica (Grupo 4)

Sistema de gerenciamento de clínica médica desenvolvido em linguagem **C** para o terminal. O projeto faz parte da Avaliação 1 (AV1) da disciplina de **Linguagem de Programação I (LP1)** do curso de Ciência da Computação / TI do **Instituto Federal do Ceará (IFCE) - Campus Tianguá**.

---

## 📌 Funcionalidades do Sistema

- [x] **Cadastrar Paciente:** Registro de código, nome, idade e telefone. *(Desenvolvido por Pedro Henrique)*
- [ ] **Cadastrar Médico:** Registro de código, nome e especialidade.
- [ ] **Agendar Consulta:** Marcação de consulta vinculada a um médico, data e horário.
- [ ] **Cancelar Consulta:** Desativação de uma consulta agendada.
- [ ] **Busca de Paciente:** Localização e exibição dos dados dos pacientes cadastrados.

---

## 🛠️ Tecnologias Utilizadas

- **Linguagem:** C (Padrão C99 / C11)
- **Compilador Recomendado:** GCC / Clang
- **Bibliotecas Padrão:** `<stdio.h>`, `<stdlib.h>`, `<string.h>`

---

## 🚀 Como Compilar e Executar

### 1. Pré-requisitos
Certifique-se de ter um compilador C instalado na sua máquina (ex: GCC no Linux/Pop!_OS ou MinGW no Windows).

### 2. Compilação
Abra o terminal no diretório do projeto e execute:

``bash
gcc -o clinica main.c

3. Execução
Após compilar, execute o programa:

Linux / macOS:

Bash
./clinica
Windows:

DOS
clinica.exe

## 🗂️ Estrutura das Estruturas (Structs)
Paciente

```
C
struct paciente {
    int codigo;
    char nome[100];
    int idade;
    char telefone[20];
};
```

## 👥 Integrantes do Grupo (Grupo 4)

```
Renan Pereira Melo
Soldhallas de Melo Fontenele

Lohan Fontenele Brito

Pedro Henrique Sousa da Silva

João Bernardo
```

🏢 Instituição
IFCE - Instituto Federal de Educação, Ciência e Tecnologia do Ceará
