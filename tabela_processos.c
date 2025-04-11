#include "tabela_processos.h"

// Inicializar a tabela de processos
TabelaProcessos* inicializar_tabela() {
    TabelaProcessos* tabela = (TabelaProcessos*)malloc(sizeof(TabelaProcessos));
    if (tabela == NULL) {
        printf("Erro: Falha na alocação de memória para a tabela\n");
        exit(1);
    }
    
    tabela->capacidade = 20000;  // Capacidade inicial para 20.000 processos
    tabela->quantidade = 0;
    
    tabela->processos = (Processo*)malloc(tabela->capacidade * sizeof(Processo));
    if (tabela->processos == NULL) {
        printf("Erro: Falha na alocação de memória para os processos\n");
        free(tabela);
        exit(1);
    }
    
    return tabela;
}


void liberar_tabela(TabelaProcessos* tabela) {
    if (tabela != NULL) {
        if (tabela->processos != NULL) {
            free(tabela->processos);
        }
        free(tabela);
    }
}

// Funções para expandir a capacidade da tabela se necessário
void expandir_tabela_se_necessario(TabelaProcessos* tabela) {
    if (tabela->quantidade >= tabela->capacidade) {
        tabela->capacidade *= 2;
        tabela->processos = (Processo*)realloc(tabela->processos, 
                                              tabela->capacidade * sizeof(Processo));
        
        if (tabela->processos == NULL) {
            printf("Erro: Falha na realocação de memória\n");
            exit(1);
        }
    }
}

// Auxiliar para remover aspas de uma string
void remover_aspas(char* str) {
    if (str == NULL) return;
    
    int len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len-1] == '"') {
        // Remover aspas do início
        memmove(str, str+1, len-1);
        // Remover aspas do final
        str[len-2] = '\0';
    }
}

// Função para ler o arquivo CSV e carregar os processos na tabela
TabelaProcessos* ler_arquivo_csv() {
    
    FILE* arquivo = fopen("processo_043_202409032338.csv", "r");
    if (arquivo == NULL) {
        printf("Erro: Não foi possível abrir o arquivo processo_043_202409032338.csv\n");
        return NULL;
    }
    
    TabelaProcessos* tabela = inicializar_tabela();
    
    char linha[1024];
    char cabecalho[1024];
    
    // Ler o cabeçalho
    if (fgets(cabecalho, sizeof(cabecalho), arquivo) == NULL) {
        printf("Erro: Arquivo vazio ou problema na leitura do cabeçalho\n");
        liberar_tabela(tabela);
        fclose(arquivo);
        return NULL;
    }
    
    
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        // Remover o caractere de nova linha, se presente
        size_t len = strlen(linha);
        if (len > 0 && (linha[len-1] == '\n' || linha[len-1] == '\r')) {
            linha[--len] = '\0';
        }
        if (len > 0 && (linha[len-1] == '\n' || linha[len-1] == '\r')) {
            linha[--len] = '\0';
        }
        
        // Ignorar linhas vazias caso tenha alguma
        if (strlen(linha) == 0) continue;
        
        // Expandir a tabela se necessário
        expandir_tabela_se_necessario(tabela);
        
        // Processar a linha para extrair os campos
        Processo processo;
        
        int campo_atual = 0;
        int dentro_de_aspas = 0;
        int inicio_campo = 0;
        char buffer[1024] = {0};
        
        for (int i = 0; i <= len; i++) {
            // Verificar se estamos dentro de aspas
            if (linha[i] == '"') {
                dentro_de_aspas = !dentro_de_aspas;
                continue;
            }
            
            // Se encontramos uma vígula fora de aspas ou o final da linha
            if ((linha[i] == ',' && !dentro_de_aspas) || i == len) {
                // Copiar o campo para o buffer
                int tamanho_campo = i - inicio_campo;
                if (tamanho_campo > 0) {
                    strncpy(buffer, linha + inicio_campo, tamanho_campo);
                    buffer[tamanho_campo] = '\0';
                } else {
                    buffer[0] = '\0';
                }
                
                // Remover aspas extras
                remover_aspas(buffer);
                
        
                switch (campo_atual) {
                    case 0: // id
                        processo.id = atoi(buffer);
                        break;
                    case 1: // numero
                        strncpy(processo.numero, buffer, sizeof(processo.numero) - 1);
                        processo.numero[sizeof(processo.numero) - 1] = '\0';
                        break;
                    case 2: // data_ajuizamento
                        strncpy(processo.data_ajuizamento, buffer, sizeof(processo.data_ajuizamento) - 1);
                        processo.data_ajuizamento[sizeof(processo.data_ajuizamento) - 1] = '\0';
                        break;
                    case 3: // id_classe
                        strncpy(processo.id_classe, buffer, sizeof(processo.id_classe) - 1);
                        processo.id_classe[sizeof(processo.id_classe) - 1] = '\0';
                        break;
                    case 4: // id_assunto
                        strncpy(processo.id_assunto, buffer, sizeof(processo.id_assunto) - 1);
                        processo.id_assunto[sizeof(processo.id_assunto) - 1] = '\0';
                        break;
                    case 5: // ano_eleicao
                        processo.ano_eleicao = atoi(buffer);
                        break;
                }
                
                // Proximo campo
                campo_atual++;
                inicio_campo = i + 1;
            }
        }
        
        tabela->processos[tabela->quantidade++] = processo;
    }
    
    fclose(arquivo);
    printf("Arquivo lido com sucesso. %d processos carregados.\n", tabela->quantidade);
    
    return tabela;
}


void trocar_processos(Processo* a, Processo* b) {
    Processo temp = *a;
    *a = *b;
    *b = temp;
}

// Utilizamos bublle sort no começo, mas o código ficou processando por muito tempo. Então utilizamos o quick sort.
int particionar_por_id(Processo* processos, int baixo, int alto) {
    int pivo = processos[alto].id;
    int i = (baixo - 1);
    
    for (int j = baixo; j <= alto - 1; j++) {
        if (processos[j].id < pivo) {
            i++;
            trocar_processos(&processos[i], &processos[j]);
        }
    }
    trocar_processos(&processos[i + 1], &processos[alto]);
    return (i + 1);
}

// QuickSort para ordenar por ID
void quicksort_por_id(Processo* processos, int baixo, int alto) {
    if (baixo < alto) {
        int pi = particionar_por_id(processos, baixo, alto);
        
        quicksort_por_id(processos, baixo, pi - 1);
        quicksort_por_id(processos, pi + 1, alto);
    }
}


void ordenar_por_id_crescente(TabelaProcessos* tabela) {
    printf("Ordenando %d processos por ID...\n", tabela->quantidade);
    quicksort_por_id(tabela->processos, 0, tabela->quantidade - 1);
    printf("Ordenação por ID concluida.\n");
}

DataCompleta converter_string_para_data(const char* data_str) {
    DataCompleta data = {0};
    
    // Formato esperado: AAAA-MM-DD HH:MM:SS.MMM
    sscanf(data_str, "%d-%d-%d %d:%d:%d.%d", 
           &data.ano, &data.mes, &data.dia, 
           &data.hora, &data.minuto, &data.segundo, &data.milissegundo);
    
    return data;
}

// Função para extrair apenas o tempo de uma string de data
Tempo extrair_tempo_da_data(const char* data_str) {
    Tempo tempo = {0};
    DataCompleta data = converter_string_para_data(data_str);
    
    tempo.hora = data.hora;
    tempo.minuto = data.minuto;
    tempo.segundo = data.segundo;
    tempo.milissegundo = data.milissegundo;
    
    return tempo;
}

// Função auxiliar para comparar datas
// Retorna: -1 se data1 < data2, 0 se data1 == data2, 1 se data1 > data2
int comparar_datas(const char* data1, const char* data2) {
    DataCompleta d1 = converter_string_para_data(data1);
    DataCompleta d2 = converter_string_para_data(data2);
    
    // Comparar ano
    if (d1.ano != d2.ano) return d1.ano < d2.ano ? -1 : 1;
    
    // Comparar mês
    if (d1.mes != d2.mes) return d1.mes < d2.mes ? -1 : 1;
    
    // Comparar dia
    if (d1.dia != d2.dia) return d1.dia < d2.dia ? -1 : 1;
    
    // Comparar hora
    if (d1.hora != d2.hora) return d1.hora < d2.hora ? -1 : 1;
    
    // Comparar minuto
    if (d1.minuto != d2.minuto) return d1.minuto < d2.minuto ? -1 : 1;
    
    // Comparar segundo
    if (d1.segundo != d2.segundo) return d1.segundo < d2.segundo ? -1 : 1;
    
    // Comparar milissegundo
    if (d1.milissegundo != d2.milissegundo) return d1.milissegundo < d2.milissegundo ? -1 : 1;
    
    // Datas iguais
    return 0;
}

int particionar_por_data_decrescente(Processo* processos, int baixo, int alto) {
    const char* pivo = processos[alto].data_ajuizamento;
    int i = (baixo - 1);
    
    for (int j = baixo; j <= alto - 1; j++) {
        
        if (comparar_datas(processos[j].data_ajuizamento, pivo) > 0) {
            i++;
            trocar_processos(&processos[i], &processos[j]);
        }
    }
    trocar_processos(&processos[i + 1], &processos[alto]);
    return (i + 1);
}


void quicksort_por_data_decrescente(Processo* processos, int baixo, int alto) {
    if (baixo < alto) {
        int pi = particionar_por_data_decrescente(processos, baixo, alto);
        
        quicksort_por_data_decrescente(processos, baixo, pi - 1);
        quicksort_por_data_decrescente(processos, pi + 1, alto);
    }
}


void ordenar_por_data_decrescente(TabelaProcessos* tabela) {
    printf("Ordenando %d processos por data de ajuizamento...\n", tabela->quantidade);
    quicksort_por_data_decrescente(tabela->processos, 0, tabela->quantidade - 1);
    printf("Ordenação por data concluida.\n");
}


int contar_processos_por_classe(TabelaProcessos* tabela, const char* id_classe) {
    int contador = 0;
    char buffer[256];
    
    for (int i = 0; i < tabela->quantidade; i++) {
        // Copiar o id_classe para um buffer
        strcpy(buffer, tabela->processos[i].id_classe);
        
        // Remover as chaves e verificar se contém o id_classe
        if (buffer[0] == '{' && buffer[strlen(buffer) - 1] == '}') {
            // Remover as chaves
            buffer[strlen(buffer) - 1] = '\0';
            memmove(buffer, buffer + 1, strlen(buffer));
            
            // Verificar se o id_classe está presente
            char* token = strtok(buffer, ",");
            while (token != NULL) {
                if (strcmp(token, id_classe) == 0) {
                    contador++;
                    break;
                }
                token = strtok(NULL, ",");
            }
        } else {
            // Fizemos um if para a verificação de chaves.
            if (strcmp(buffer, id_classe) == 0) {
                contador++;
            }
        }
    }
    
    return contador;
}


int contar_id_assuntos_unicos(TabelaProcessos* tabela) {
    // Usaremos um array para armazenar os id_assuntos únicos
    char** assuntos_unicos = (char**)malloc(tabela->quantidade * 10 * sizeof(char*));
    int num_assuntos_unicos = 0;
    
    if (assuntos_unicos == NULL) {
        printf("Erro: Falha na alocação de memória\n");
        return -1;
    }
    
    for (int i = 0; i < tabela->quantidade; i++) {
        char buffer[256];
        strcpy(buffer, tabela->processos[i].id_assunto);
        
        // Remover as chaves se existirem
        if (buffer[0] == '{' && buffer[strlen(buffer) - 1] == '}') {
            buffer[strlen(buffer) - 1] = '\0';
            memmove(buffer, buffer + 1, strlen(buffer));
            
            // Separar os assuntos por vírgula
            char* token = strtok(buffer, ",");
            while (token != NULL) {
                // Verificar se este assunto já esta na lista de únicos
                int ja_existe = 0;
                for (int j = 0; j < num_assuntos_unicos; j++) {
                    if (strcmp(assuntos_unicos[j], token) == 0) {
                        ja_existe = 1;
                        break;
                    }
                }
                
                
                if (!ja_existe) {
                    assuntos_unicos[num_assuntos_unicos] = strdup(token);
                    num_assuntos_unicos++;
                }
                
                token = strtok(NULL, ",");
            }
        } else {
            
            int ja_existe = 0;
            for (int j = 0; j < num_assuntos_unicos; j++) {
                if (strcmp(assuntos_unicos[j], buffer) == 0) {
                    ja_existe = 1;
                    break;
                }
            }
            
            
            if (!ja_existe) {
                assuntos_unicos[num_assuntos_unicos] = strdup(buffer);
                num_assuntos_unicos++;
            }
        }
    }
    
    
    for (int i = 0; i < num_assuntos_unicos; i++) {
        free(assuntos_unicos[i]);
    }
    free(assuntos_unicos);
    
    return num_assuntos_unicos;
}


void listar_processos_multiplos_assuntos(TabelaProcessos* tabela) {
    printf("Esses são os Processos vinculados a mais de um assunto:\n");
    
    for (int i = 0; i < tabela->quantidade; i++) {
        char buffer[256];
        strcpy(buffer, tabela->processos[i].id_assunto);
        
        // Verificar se o campo id_assunto tem multivalorados considerando as vírgulas entre as chaves
        if (buffer[0] == '{' && buffer[strlen(buffer) - 1] == '}') {
            // Removendo as Chaves
            buffer[strlen(buffer) - 1] = '\0';
            memmove(buffer, buffer + 1, strlen(buffer));
            
            // Contar quantos assuntos existem
            int num_assuntos = 1;  // Todos tem pelo menos um assunto
            for (int j = 0; buffer[j] != '\0'; j++) {
                if (buffer[j] == ',') {
                    num_assuntos++;
                }
            }
            
            // Se tiver mais de um assunto, imprimir
            if (num_assuntos > 1) {
                printf("ID: %d, Número: %s, Assuntos: %s\n", 
                       tabela->processos[i].id, 
                       tabela->processos[i].numero, 
                       tabela->processos[i].id_assunto);
            }
        }
    }
}


Processo* encontrar_processo_maior_tramitacao(TabelaProcessos* tabela) {
    if (tabela->quantidade == 0) return NULL;
    
    Processo* processo_maior_tramitacao = &tabela->processos[0];
    Tempo tempo_maior = extrair_tempo_da_data(processo_maior_tramitacao->data_ajuizamento);
    
    for (int i = 1; i < tabela->quantidade; i++) {
        Tempo tempo_atual = extrair_tempo_da_data(tabela->processos[i].data_ajuizamento);
        
        // Comparar apenas o tempo (hora, minuto, segundo, milissegundo)
        if (tempo_atual.hora > tempo_maior.hora || 
            (tempo_atual.hora == tempo_maior.hora && tempo_atual.minuto > tempo_maior.minuto) || 
            (tempo_atual.hora == tempo_maior.hora && tempo_atual.minuto == tempo_maior.minuto && tempo_atual.segundo > tempo_maior.segundo) || 
            (tempo_atual.hora == tempo_maior.hora && tempo_atual.minuto == tempo_maior.minuto && tempo_atual.segundo == tempo_maior.segundo && tempo_atual.milissegundo > tempo_maior.milissegundo)) {
            processo_maior_tramitacao = &tabela->processos[i];
            tempo_maior = tempo_atual;
        }
    }
    
    return processo_maior_tramitacao;
}


void salvar_tabela_csv(TabelaProcessos* tabela, const char* nome_arquivo, const char* cabecalho) {
    FILE* arquivo = fopen(nome_arquivo, "w"); // Write para gravaÃ§Ã£o do arquivo
    if (arquivo == NULL) {
        printf("Erro: NÃO foi possível criar o arquivo %s\n", nome_arquivo);
        return;
    }
    
    
    fprintf(arquivo, "%s\n", cabecalho);
    
    
    for (int i = 0; i < tabela->quantidade; i++) {
        fprintf(arquivo, "%d,\"%s\",%s,%s,%s,%d\n",
                tabela->processos[i].id,
                tabela->processos[i].numero,
                tabela->processos[i].data_ajuizamento,
                tabela->processos[i].id_classe,
                tabela->processos[i].id_assunto,
                tabela->processos[i].ano_eleicao);
    }
    
    fclose(arquivo);
    printf("Arquivo %s salvo com sucesso.\n", nome_arquivo);
}


void imprimir_todos_processos_com_tempos(TabelaProcessos* tabela) {
    printf("\nListagem de todos os processos com seus tempos de tramitação:\n");
    printf("ID\tNúmero\t\t\tData Ajuizamento\t\tTempo de Tramitação\n");
    printf("-----------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < tabela->quantidade; i++) {
        Tempo tempo = extrair_tempo_da_data(tabela->processos[i].data_ajuizamento);
        printf("%d\t%s\t%s\t%02d:%02d:%02d.%03d\n", 
               tabela->processos[i].id, 
               tabela->processos[i].numero, 
               tabela->processos[i].data_ajuizamento,
               tempo.hora, tempo.minuto, tempo.segundo, tempo.milissegundo);
    }
}
