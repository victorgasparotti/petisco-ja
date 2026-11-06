#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <windows.h>

MYSQL *conexao;

// Cadastrar produto
void cadastrarProduto() {
    char nome[100];
    char descricao[200];
    char comando[600];
    int categoria;
    float preco;

    printf("\n===== CADASTRAR PRODUTO =====\n");

    printf("Nome: ");
    scanf(" %99[^\n]", nome);

    printf("Descricao: ");
    scanf(" %199[^\n]", descricao);

    printf("Preco: ");
    scanf("%f", &preco);

    printf("ID da categoria: ");
    scanf("%d", &categoria);

    if (preco <= 0 || categoria <= 0) {
        printf("\nPreco ou categoria invalida!\n");
        return;
    }

    snprintf(comando, sizeof(comando),
        "INSERT INTO produto "
        "(id_categoria, nome, descricao, preco, ativo) "
        "VALUES (%d, '%s', '%s', %.2f, 1)",
        categoria, nome, descricao, preco
    );

    if (mysql_query(conexao, comando) == 0) {
        printf("\nProduto cadastrado com sucesso!\n");
    } else {
        printf("\nErro ao cadastrar: %s\n", mysql_error(conexao));
    }
}

// Listar produtos
void listarProdutos() {
    MYSQL_RES *resultado;
    MYSQL_ROW linha;

    char comando[] =
        "SELECT produto.id_produto, produto.nome, "
        "categoria.nome, produto.preco, produto.ativo "
        "FROM produto "
        "INNER JOIN categoria "
        "ON produto.id_categoria = categoria.id_categoria "
        "ORDER BY produto.id_produto ASC";

    if (mysql_query(conexao, comando) != 0) {
        printf("\nErro ao consultar produtos!\n");
        return;
    }

    resultado = mysql_store_result(conexao);

    if (resultado == NULL) {
        printf("\nErro ao obter os dados!\n");
        return;
    }

    printf("\n===== PRODUTOS CADASTRADOS =====\n");

    if (mysql_num_rows(resultado) == 0) {
        printf("Nenhum produto cadastrado.\n");
    }

    while ((linha = mysql_fetch_row(resultado))) {
        printf("\nID: %s\n", linha[0]);
        printf("Nome: %s\n", linha[1]);
        printf("Categoria: %s\n", linha[2]);
        printf("Preco: R$ %s\n", linha[3]);
        printf("Ativo: %s\n", linha[4]);
    }

    mysql_free_result(resultado);
}

// Consultar categorias
void consultarCategorias() {
    MYSQL_RES *resultado;
    MYSQL_ROW linha;

    if (mysql_query(conexao,
        "SELECT id_categoria, nome FROM categoria") != 0) {
        printf("\nErro ao consultar categorias!\n");
        return;
    }

    resultado = mysql_store_result(conexao);

    if (resultado == NULL) {
        printf("\nErro ao obter categorias!\n");
        return;
    }

    printf("\n===== CATEGORIAS =====\n");

    while ((linha = mysql_fetch_row(resultado))) {
        printf("ID: %s | Categoria: %s\n", linha[0], linha[1]);
    }

    mysql_free_result(resultado);
}

// Editar produto
void editarProduto() {
    int id;
    int categoria;
    float preco;

    char nome[100];
    char descricao[200];
    char entrada[100];
    char comando[700];

    printf("\n===== EDITAR PRODUTO =====\n");
    printf("Digite 'sair' a qualquer momento para voltar.\n\n");

    printf("ID do produto: ");
    scanf(" %99[^\n]", entrada);

    if (strcasecmp(entrada, "sair") == 0) {
        printf("\nVoltando ao menu principal...\n");
        return;
    }

    id = atoi(entrada);

    if (id <= 0) {
        printf("\nID invalido!\n");
        return;
    }

    printf("Novo nome: ");
    scanf(" %99[^\n]", nome);

    if (strcasecmp(nome, "sair") == 0) {
        printf("\nVoltando ao menu principal...\n");
        return;
    }

    printf("Nova descricao: ");
    scanf(" %199[^\n]", descricao);

    if (strcasecmp(descricao, "sair") == 0) {
        printf("\nVoltando ao menu principal...\n");
        return;
    }

    printf("Novo preco: ");
    scanf(" %99[^\n]", entrada);

    if (strcasecmp(entrada, "sair") == 0) {
        printf("\nVoltando ao menu principal...\n");
        return;
    }

    preco = atof(entrada);

    printf("Novo ID da categoria: ");
    scanf(" %99[^\n]", entrada);

    if (strcasecmp(entrada, "sair") == 0) {
        printf("\nVoltando ao menu principal...\n");
        return;
    }

    categoria = atoi(entrada);

    if (preco <= 0 || categoria <= 0) {
        printf("\nDados invalidos!\n");
        return;
    }

    snprintf(comando, sizeof(comando),
        "UPDATE produto SET nome='%s', descricao='%s', "
        "preco=%.2f, id_categoria=%d "
        "WHERE id_produto=%d",
        nome, descricao, preco, categoria, id
    );

    if (mysql_query(conexao, comando) == 0) {
        if (mysql_affected_rows(conexao) > 0) {
            printf("\nProduto atualizado com sucesso!\n");
        } else {
            printf("\nNenhum produto foi alterado. Verifique o ID.\n");
        }
    } else {
        printf("\nErro ao editar: %s\n", mysql_error(conexao));
    }
}



// Excluir produto
void excluirProduto() {
    int id;
    char comando[200];

    printf("\n===== EXCLUIR PRODUTO =====\n");

    printf("ID do produto: ");
    scanf("%d", &id);

    if (id <= 0) {
        printf("\nID invalido!\n");
        return;
    }

    snprintf(comando, sizeof(comando),
        "DELETE FROM produto WHERE id_produto=%d", id
    );

    if (mysql_query(conexao, comando) == 0) {
        if (mysql_affected_rows(conexao) > 0) {
            printf("\nProduto excluido com sucesso!\n");
        } else {
            printf("\nProduto nao encontrado!\n");
        }
    } else {
        printf("\nNao foi possivel excluir o produto.\n");
        printf("Motivo: %s\n", mysql_error(conexao));
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    
    int opcao;

    conexao = mysql_init(NULL);

    if (conexao == NULL) {
        printf("Erro ao iniciar MySQL!\n");
        return 1;
    }

    // Mantenha aqui os dados de conexao que ja funcionaram
    if (mysql_real_connect(
        conexao,
        "localhost",
        "root",
        "Golo$4955",
        "petisco_ja",
        3306,
        NULL,
        0
    ) == NULL) {

        printf("Erro na conexao: %s\n", mysql_error(conexao));
        mysql_close(conexao);
        return 1;
    }

    mysql_set_character_set(conexao, "utf8mb4");

    printf("\nConectado ao banco Petisco Ja!\n");

    do {
        printf("\n===== PETISCO JA =====\n");
        printf("1 - Cadastrar produto\n");
        printf("2 - Listar produtos\n");
        printf("3 - Editar produto\n");
        printf("4 - Excluir produto\n");
        printf("5 - Consultar categorias\n");
        printf("0 - Sair\n");

        printf("\nEscolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                cadastrarProduto();
                break;

            case 2:
                listarProdutos();
                break;

            case 3:
                editarProduto();
                break;

            case 4:
                excluirProduto();
                break;

            case 5:
                consultarCategorias();
                break;

            case 0:
                printf("\nEncerrando sistema...\n");
                break;

            default:
                printf("\nOpção invalida!\n");
        }

    } while (opcao != 0);

    mysql_close(conexao);

    return 0;
}
