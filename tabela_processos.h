#ifndef TABELA_PROCESSOS_H
#define TABELA_PROCESSOS_H

// Inclusões necessárias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>  // Adicionado para suporte a locale

// Definição do TAD Processo
typedef struct {
    int id;                     // ID do processo
    char numero[30];            // Número do processo
    char data_ajuizamento[30];  // Data de ajuizamento
    char id_classe[256];        // ID da classe (pode ser multivalorado)
    char id_assunto[256];       // ID do assunto (pode ser multivalorado)
    int ano_eleicao;            // Ano da eleiçao
} Processo;

// Estrutura para armazenar a tabela de processos
typedef struct {
    Processo* processos;        // Vetor dinamico de processos
    int quantidade;             // Quantidade de processos na tabela
    int capacidade;             // Capacidade atual do vetor
} TabelaProcessos;


typedef struct {
    int ano;
    int mes;
    int dia;
    int hora;
    int minuto;
    int segundo;
    int milissegundo;
} DataCompleta;

// Estrutura para armazenar apenas o tempo
typedef struct {
    int hora;
    int minuto;
    int segundo;
    int milissegundo;
} Tempo;



// Funções de inicializão e gerenciamento da tabela
TabelaProcessos* inicializar_tabela();
void liberar_tabela(TabelaProcessos* tabela);
void expandir_tabela_se_necessario(TabelaProcessos* tabela);


void remover_aspas(char* str);
DataCompleta converter_string_para_data(const char* data_str);
Tempo extrair_tempo_da_data(const char* data_str);
int comparar_datas(const char* data1, const char* data2);
void trocar_processos(Processo* a, Processo* b);

// Funções de leitura e escrita de arquivos
TabelaProcessos* ler_arquivo_csv();
void salvar_tabela_csv(TabelaProcessos* tabela, const char* nome_arquivo, const char* cabecalho);

// Funções de ordenação
int particionar_por_id(Processo* processos, int baixo, int alto);
void quicksort_por_id(Processo* processos, int baixo, int alto);
void ordenar_por_id_crescente(TabelaProcessos* tabela);
int particionar_por_data_decrescente(Processo* processos, int baixo, int alto);
void quicksort_por_data_decrescente(Processo* processos, int baixo, int alto);
void ordenar_por_data_decrescente(TabelaProcessos* tabela);


int contar_processos_por_classe(TabelaProcessos* tabela, const char* id_classe);
int contar_id_assuntos_unicos(TabelaProcessos* tabela);
void listar_processos_multiplos_assuntos(TabelaProcessos* tabela);
Processo* encontrar_processo_maior_tramitacao(TabelaProcessos* tabela);
void imprimir_todos_processos_com_tempos(TabelaProcessos* tabela);

#endif // TABELA_PROCESSOS_H
