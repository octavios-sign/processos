#include "tabela_processos.h"

int main() {
    
    setlocale(LC_ALL, "Portuguese");
    
    printf("Iniciando processamento de dados judiciais...\n");
    
    // Ler o arquivo CSV
    TabelaProcessos* tabela = ler_arquivo_csv();
    if (tabela == NULL) {
        printf("Ops! O c�digo n�o conseguiu ler o Arquivo. Erro!.\n");
        return 1;
    }
    
    // cabe�alho do arquivo 
    FILE* arquivo_original = fopen("processo_043_202409032338.csv", "r");
    char cabecalho[1024];
    fgets(cabecalho, sizeof(cabecalho), arquivo_original); // Armazenando cabecalho em um Array lendo a primeira linha
    cabecalho[strcspn(cabecalho, "\n")] = 0; // Corta a String em \0 a primeira linha 
    fclose(arquivo_original);
    
   
    printf("\n1. Ordenar, em ordem crescente, o conjunto de dados a partir do atributo \"id\";\n");
    ordenar_por_id_crescente(tabela);
    salvar_tabela_csv(tabela, "processos_ordenados_id_crescente.csv", cabecalho);
    
    
    printf("\n2. Ordenar, em ordem decrescente, o conjunto de dados a partir do atributo \"data_ajuizamento\";\n");
    ordenar_por_data_decrescente(tabela);
    salvar_tabela_csv(tabela, "processos_ordenados_data_decrescente.csv", cabecalho);
    
    
    printf("\n3. Contar quantos processos est�o vinculados a um determinado \"id_classe\";\n");
    const char* classe_escolhida = "12559";
    int num_processos_classe = contar_processos_por_classe(tabela, classe_escolhida);
    printf("N�mero de processos vinculados � classe %s: %d\n", classe_escolhida, num_processos_classe);
    
    
    printf("\n4. Identificar quantos \"id_assuntos\" constam nos processos presentes na base de dados;\n");
    int num_assuntos_unicos = contar_id_assuntos_unicos(tabela);
    printf("N�mero de id_assuntos �nicos: %d\n", num_assuntos_unicos);
    
    
    printf("\n5. Listar todos os processos que est�o vinculados a mais de um assunto; e\n");
    listar_processos_multiplos_assuntos(tabela);
    
    
    printf("\n6. Indicar o processo que est� h� mais tempo em tramita��o na justi�a.\n");
    Processo* processo_maior_tramitacao = encontrar_processo_maior_tramitacao(tabela);
    if (processo_maior_tramitacao != NULL) {
        Tempo tempo = extrair_tempo_da_data(processo_maior_tramitacao->data_ajuizamento);
        printf("O processo com maior tempo de tramita��o  %s (ID: %d), ajuizado em %s\n", 
               processo_maior_tramitacao->numero, 
               processo_maior_tramitacao->id,
               processo_maior_tramitacao->data_ajuizamento);
        printf("Tempo de tramita��o: %02d:%02d:%02d.%03d\n", 
               tempo.hora, tempo.minuto, tempo.segundo, tempo.milissegundo);
    }
    
    
    liberar_tabela(tabela);
    
    printf("\nO Processamento da Tabela foi concluidos com sucesso!\n");
    
    return 0;
}
