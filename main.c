#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mysql.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define TAM_NOME 101
#define TAM_TEXTO 256
#define MAX_ITENS 30

static MYSQL *conexao = NULL;

/* Le uma linha completa, evitando problemas com espacos nos nomes. */
static void lerTexto(const char *mensagem, char *destino, size_t tamanho) {
    printf("%s", mensagem);
    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    destino[strcspn(destino, "\r\n")] = '\0';
}

static int lerInteiro(const char *mensagem) {
    char entrada[64], *fim;
    long valor;
    for (;;) {
        lerTexto(mensagem, entrada, sizeof(entrada));
        valor = strtol(entrada, &fim, 10);
        while (isspace((unsigned char)*fim)) fim++;
        if (entrada[0] != '\0' && *fim == '\0' && valor >= 0 && valor <= 2147483647L)
            return (int)valor;
        printf("Valor invalido. Digite um numero inteiro.\n");
    }
}

static double lerDecimal(const char *mensagem) {
    char entrada[64], *fim;
    double valor;
    for (;;) {
        lerTexto(mensagem, entrada, sizeof(entrada));
        for (char *p = entrada; *p; p++) if (*p == ',') *p = '.';
        valor = strtod(entrada, &fim);
        while (isspace((unsigned char)*fim)) fim++;
        if (entrada[0] != '\0' && *fim == '\0' && valor >= 0.0)
            return valor;
        printf("Valor invalido. Digite um numero maior ou igual a zero.\n");
    }
}

static void escapar(const char *origem, char *destino, size_t tamanho) {
    unsigned long n = mysql_real_escape_string(conexao, destino, origem, (unsigned long)strlen(origem));
    if (n >= tamanho) destino[tamanho - 1] = '\0';
    else destino[n] = '\0';
}

static int executarSQL(const char *sql) {
    if (mysql_query(conexao, sql) != 0) {
        printf("\nErro no banco de dados: %s\n", mysql_error(conexao));
        return 0;
    }
    return 1;
}

static MYSQL_RES *consultarSQL(const char *sql) {
    if (!executarSQL(sql)) return NULL;
    MYSQL_RES *resultado = mysql_store_result(conexao);
    if (resultado == NULL && mysql_field_count(conexao) != 0)
        printf("Erro ao receber os dados: %s\n", mysql_error(conexao));
    return resultado;
}

static int registroExiste(const char *tabela, const char *campo, int id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT 1 FROM %s WHERE %s=%d LIMIT 1", tabela, campo, id);
    MYSQL_RES *r = consultarSQL(sql);
    if (!r) return 0;
    int existe = mysql_num_rows(r) > 0;
    mysql_free_result(r);
    return existe;
}

static void pausar(void) {
    char lixo[8];
    printf("\nPressione ENTER para continuar...");
    fgets(lixo, sizeof(lixo), stdin);
}

/* Cadastro de endereco e implementado junto ao fluxo de pedidos. */
static void cadastrarEndereco(void);

/* ======================== CLIENTES ======================== */
static void cadastrarCliente(void) {
    char nome[TAM_NOME], telefone[32], email[121];
    char n[2 * TAM_NOME + 1], t[64], e[242], sql[700];
    printf("\n--- CADASTRAR CLIENTE ---\n");
    lerTexto("Nome completo: ", nome, sizeof(nome));
    lerTexto("Telefone: ", telefone, sizeof(telefone));
    lerTexto("E-mail (opcional): ", email, sizeof(email));
    if (!nome[0] || !telefone[0]) { printf("Nome e telefone sao obrigatorios.\n"); return; }
    escapar(nome, n, sizeof(n)); escapar(telefone, t, sizeof(t)); escapar(email, e, sizeof(e));
    if (email[0])
        snprintf(sql, sizeof(sql),
            "INSERT INTO cliente (nome, telefone, email) VALUES ('%s','%s','%s')", n, t, e);
    else
        snprintf(sql, sizeof(sql),
            "INSERT INTO cliente (nome, telefone, email) VALUES ('%s','%s',NULL)", n, t);
    if (executarSQL(sql)) printf("Cliente cadastrado! ID: %llu\n", mysql_insert_id(conexao));
}

static void listarClientes(void) {
    MYSQL_RES *r = consultarSQL("SELECT id_cliente,nome,telefone,COALESCE(email,'-'),DATE_FORMAT(data_cadastro,'%d/%m/%Y %H:%i') FROM cliente ORDER BY nome");
    if (!r) return;
    MYSQL_ROW row;
    printf("\n--- CLIENTES ---\n");
    if (!mysql_num_rows(r)) printf("Nenhum cliente cadastrado.\n");
    while ((row = mysql_fetch_row(r)))
        printf("ID: %-5s | %-28s | %-18s | %-28s | %s\n", row[0], row[1], row[2], row[3], row[4]);
    mysql_free_result(r);
}

static void editarCliente(void) {
    int id = lerInteiro("ID do cliente: ");
    if (!id || !registroExiste("cliente", "id_cliente", id)) { printf("Cliente nao encontrado.\n"); return; }
    char nome[TAM_NOME], telefone[32], email[121], n[202], t[64], e[242], sql[700];
    lerTexto("Novo nome: ", nome, sizeof(nome));
    lerTexto("Novo telefone: ", telefone, sizeof(telefone));
    lerTexto("Novo e-mail (deixe vazio para remover): ", email, sizeof(email));
    if (!nome[0] || !telefone[0]) { printf("Nome e telefone sao obrigatorios.\n"); return; }
    escapar(nome,n,sizeof(n)); escapar(telefone,t,sizeof(t)); escapar(email,e,sizeof(e));
    if (email[0]) snprintf(sql,sizeof(sql),"UPDATE cliente SET nome='%s',telefone='%s',email='%s' WHERE id_cliente=%d",n,t,e,id);
    else snprintf(sql,sizeof(sql),"UPDATE cliente SET nome='%s',telefone='%s',email=NULL WHERE id_cliente=%d",n,t,id);
    if (executarSQL(sql)) printf("Cliente atualizado.\n");
}

static void excluirCliente(void) {
    int id = lerInteiro("ID do cliente: ");
    if (!id) return;
    char sql[160];
    snprintf(sql,sizeof(sql),"DELETE FROM cliente WHERE id_cliente=%d",id);
    if (executarSQL(sql)) {
        if (mysql_affected_rows(conexao)) printf("Cliente excluido.\n");
        else printf("Cliente nao encontrado.\n");
    }
}

static void menuClientes(void) {
    int op;
    do {
        printf("\n===== CLIENTES =====\n1 - Cadastrar\n2 - Listar\n3 - Editar\n4 - Excluir\n5 - Cadastrar endereco de entrega\n0 - Voltar\n");
        op = lerInteiro("Opcao: ");
        switch(op) { case 1: cadastrarCliente(); break; case 2: listarClientes(); break;
            case 3: editarCliente(); break; case 4: excluirCliente(); break; case 5: cadastrarEndereco(); break; case 0: break;
            default: printf("Opcao invalida.\n"); }
        if (op) pausar();
    } while (op != 0);
}

/* ======================== PRODUTOS ======================== */
static void consultarCategorias(void) {
    MYSQL_RES *r = consultarSQL("SELECT id_categoria,nome FROM categoria ORDER BY nome");
    if (!r) return;
    MYSQL_ROW row;
    printf("\n--- CATEGORIAS ---\n");
    while ((row = mysql_fetch_row(r))) printf("%s - %s\n", row[0], row[1]);
    mysql_free_result(r);
}

static void cadastrarProduto(void) {
    char nome[101], descricao[256], n[202], d[512], sql[900];
    printf("\n--- CADASTRAR PRODUTO ---\n");
    consultarCategorias();
    int categoria = lerInteiro("ID da categoria: ");
    if (!categoria || !registroExiste("categoria","id_categoria",categoria)) { printf("Categoria inexistente.\n"); return; }
    lerTexto("Nome: ",nome,sizeof(nome)); lerTexto("Descricao: ",descricao,sizeof(descricao));
    double preco = lerDecimal("Preco (R$): ");
    if (!nome[0]) { printf("Nome obrigatorio.\n"); return; }
    escapar(nome,n,sizeof(n)); escapar(descricao,d,sizeof(d));
    snprintf(sql,sizeof(sql),"INSERT INTO produto (id_categoria,nome,descricao,preco,ativo) VALUES (%d,'%s','%s',%.2f,TRUE)",categoria,n,d,preco);
    if (executarSQL(sql)) printf("Produto cadastrado! ID: %llu\n",mysql_insert_id(conexao));
}

static void listarProdutos(void) {
    MYSQL_RES *r = consultarSQL("SELECT p.id_produto,p.nome,c.nome,p.preco,IF(p.ativo='1','Sim','Nao') FROM produto p JOIN categoria c ON c.id_categoria=p.id_categoria ORDER BY p.nome");
    if (!r) return;
    MYSQL_ROW row; printf("\n--- PRODUTOS ---\n");
    if (!mysql_num_rows(r)) printf("Nenhum produto cadastrado.\n");
    while ((row=mysql_fetch_row(r))) printf("ID: %-4s | %-25s | %-18s | R$ %-9s | Ativo: %s\n",row[0],row[1],row[2],row[3],row[4]);
    mysql_free_result(r);
}

static void editarProduto(void) {
    listarProdutos(); int id=lerInteiro("ID do produto para editar: ");
    if (!id || !registroExiste("produto","id_produto",id)) { printf("Produto nao encontrado.\n"); return; }
    char nome[101],desc[256],n[202],d[512],sql[900];
    consultarCategorias(); int cat=lerInteiro("Novo ID da categoria: ");
    if (!cat || !registroExiste("categoria","id_categoria",cat)) { printf("Categoria inexistente.\n"); return; }
    lerTexto("Novo nome: ",nome,sizeof(nome)); lerTexto("Nova descricao: ",desc,sizeof(desc));
    double preco=lerDecimal("Novo preco: "); int ativo=lerInteiro("Ativo? (1=sim / 0=nao): ");
    if (!nome[0] || ativo>1) { printf("Dados invalidos.\n"); return; }
    escapar(nome,n,sizeof(n)); escapar(desc,d,sizeof(d));
    snprintf(sql,sizeof(sql),"UPDATE produto SET id_categoria=%d,nome='%s',descricao='%s',preco=%.2f,ativo=%d WHERE id_produto=%d",cat,n,d,preco,ativo,id);
    if (executarSQL(sql)) printf("Produto atualizado.\n");
}

static void excluirProduto(void) {
    listarProdutos(); int id=lerInteiro("ID do produto para excluir: ");
    if (!id) return;
    char sql[180]; snprintf(sql,sizeof(sql),"DELETE FROM produto WHERE id_produto=%d",id);
    if (executarSQL(sql)) {
        if (mysql_affected_rows(conexao)) printf("Produto excluido.\n");
        else printf("Produto nao encontrado.\n");
    }
}

static void menuProdutos(void) {
    int op;
    do {
        printf("\n===== PRODUTOS =====\n1 - Cadastrar\n2 - Listar\n3 - Editar\n4 - Excluir\n5 - Consultar categorias\n0 - Voltar\n");
        op=lerInteiro("Opcao: ");
        switch(op) {case 1:cadastrarProduto();break;case 2:listarProdutos();break;case 3:editarProduto();break;case 4:excluirProduto();break;case 5:consultarCategorias();break;case 0:break;default:printf("Opcao invalida.\n");}
        if(op) pausar();
    } while(op!=0);
}

/* ======================== ESTOQUE / INSUMOS ======================== */
static void listarEstoque(void) {
    MYSQL_RES *r=consultarSQL("SELECT id_insumo,nome,unidade_medida,qtd_atual,qtd_minima,IF(qtd_atual<qtd_minima,'REPOR','OK') FROM insumo ORDER BY nome");
    if(!r)return; MYSQL_ROW row; printf("\n--- ESTOQUE DE INSUMOS ---\n");
    if(!mysql_num_rows(r)) printf("Nenhum insumo cadastrado.\n");
    while((row=mysql_fetch_row(r))) printf("ID: %-4s | %-24s | Atual: %-10s %s | Minimo: %-10s | %s\n",row[0],row[1],row[3],row[2],row[4],row[5]);
    mysql_free_result(r);
}

static void cadastrarInsumo(void) {
    char nome[101],unidade[11],n[202],u[32],sql[500];
    printf("\n--- CADASTRAR INSUMO ---\n");
    lerTexto("Nome do insumo: ",nome,sizeof(nome)); lerTexto("Unidade (kg, L, un...): ",unidade,sizeof(unidade));
    double minimo=lerDecimal("Quantidade minima: ");
    if(!nome[0]||!unidade[0]){printf("Nome e unidade sao obrigatorios.\n");return;}
    escapar(nome,n,sizeof(n));escapar(unidade,u,sizeof(u));
    snprintf(sql,sizeof(sql),"INSERT INTO insumo (nome,unidade_medida,qtd_atual,qtd_minima) VALUES ('%s','%s',0,%.3f)",n,u,minimo);
    if(executarSQL(sql))printf("Insumo cadastrado! ID: %llu\n",mysql_insert_id(conexao));
}

static void movimentarEstoque(void) {
    listarEstoque(); int id=lerInteiro("ID do insumo: ");
    if(!id||!registroExiste("insumo","id_insumo",id)){printf("Insumo nao encontrado.\n");return;}
    int usuario=lerInteiro("ID do usuario responsavel (tabela usuario): ");
    if(!usuario||!registroExiste("usuario","id_usuario",usuario)){printf("Usuario nao encontrado.\n");return;}
    printf("1 - ENTRADA\n2 - SAIDA\n"); int tipo=lerInteiro("Tipo: ");
    if(tipo!=1&&tipo!=2){printf("Tipo invalido.\n");return;}
    double qtd;
    do { qtd=lerDecimal("Quantidade: "); if(qtd<=0)printf("A quantidade deve ser maior que zero.\n"); } while(qtd<=0);
    char sql[600];
    if(!executarSQL("START TRANSACTION"))return;
    snprintf(sql,sizeof(sql),"UPDATE insumo SET qtd_atual=qtd_atual %c %.3f WHERE id_insumo=%d",tipo==1?'+':'-',qtd,id);
    if(!executarSQL(sql)){executarSQL("ROLLBACK");return;}
    snprintf(sql,sizeof(sql),"INSERT INTO movimentacao_estoque (id_insumo,id_usuario,tipo,quantidade) VALUES (%d,%d,'%s',%.3f)",id,usuario,tipo==1?"ENTRADA":"SAIDA",qtd);
    if(!executarSQL(sql)){executarSQL("ROLLBACK");return;}
    if(!executarSQL("COMMIT")){executarSQL("ROLLBACK");return;}
    printf("Movimentacao registrada com sucesso.\n");
}

static void historicoEstoque(void) {
    MYSQL_RES *r=consultarSQL("SELECT m.id_movimentacao,i.nome,m.tipo,m.quantidade,i.unidade_medida,u.nome,DATE_FORMAT(m.data_hora,'%d/%m/%Y %H:%i') FROM movimentacao_estoque m JOIN insumo i ON i.id_insumo=m.id_insumo JOIN usuario u ON u.id_usuario=m.id_usuario ORDER BY m.data_hora DESC LIMIT 100");
    if(!r)return; MYSQL_ROW row; printf("\n--- ULTIMAS 100 MOVIMENTACOES ---\n");
    if(!mysql_num_rows(r))printf("Nenhuma movimentacao registrada.\n");
    while((row=mysql_fetch_row(r)))printf("#%s | %-20s | %-7s | %s %s | Resp.: %-18s | %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
    mysql_free_result(r);
}

static void configurarFichaTecnica(void) {
    printf("\n--- FICHA TECNICA DO PRODUTO ---\n");
    listarProdutos();
    int produto=lerInteiro("ID do produto: ");
    if(!produto||!registroExiste("produto","id_produto",produto)){printf("Produto nao encontrado.\n");return;}
    listarEstoque();
    int insumo=lerInteiro("ID do insumo: ");
    if(!insumo||!registroExiste("insumo","id_insumo",insumo)){printf("Insumo nao encontrado.\n");return;}
    double quantidade;
    do { quantidade=lerDecimal("Quantidade deste insumo usada em UMA unidade do produto: "); if(quantidade<=0)printf("A quantidade deve ser maior que zero.\n"); } while(quantidade<=0);
    char sql[500];
    snprintf(sql,sizeof(sql),"INSERT INTO produto_insumo (id_produto,id_insumo,qtd_utilizada) VALUES (%d,%d,%.3f) ON DUPLICATE KEY UPDATE qtd_utilizada=VALUES(qtd_utilizada)",produto,insumo,quantidade);
    if(executarSQL(sql))printf("Ficha tecnica salva. A baixa automatica podera calcular o consumo deste insumo.\n");
}

static void listarFichasTecnicas(void) {
    MYSQL_RES *r=consultarSQL("SELECT p.nome,i.nome,pi.qtd_utilizada,i.unidade_medida FROM produto_insumo pi JOIN produto p ON p.id_produto=pi.id_produto JOIN insumo i ON i.id_insumo=pi.id_insumo ORDER BY p.nome,i.nome");
    if(!r)return;
    MYSQL_ROW row; printf("\n--- FICHAS TECNICAS ---\n");
    if(!mysql_num_rows(r))printf("Nenhuma ficha tecnica cadastrada.\n");
    while((row=mysql_fetch_row(r)))printf("Produto: %-24s | Insumo: %-20s | Consumo: %s %s\n",row[0],row[1],row[2],row[3]);
    mysql_free_result(r);
}

static void menuEstoque(void) {
    int op;
    do {
        printf("\n===== ESTOQUE =====\n1 - Cadastrar insumo\n2 - Consultar estoque\n3 - Registrar entrada/saida\n4 - Historico de movimentacoes\n5 - Configurar ficha tecnica de produto\n6 - Consultar fichas tecnicas\n0 - Voltar\n");
        op=lerInteiro("Opcao: ");
        switch(op){case 1:cadastrarInsumo();break;case 2:listarEstoque();break;case 3:movimentarEstoque();break;case 4:historicoEstoque();break;case 5:configurarFichaTecnica();break;case 6:listarFichasTecnicas();break;case 0:break;default:printf("Opcao invalida.\n");}
        if(op)pausar();
    }while(op!=0);
}

/* ======================== PEDIDOS ======================== */
typedef struct { int id_produto; int quantidade; double preco; char nome[101]; } ItemPedido;

static void listarEnderecosCliente(int id_cliente) {
    char sql[500]; snprintf(sql,sizeof(sql),"SELECT id_endereco,logradouro,numero,bairro,cidade,uf FROM endereco WHERE id_cliente=%d ORDER BY id_endereco",id_cliente);
    MYSQL_RES *r=consultarSQL(sql); if(!r)return; MYSQL_ROW row;
    printf("\n--- ENDERECOS DO CLIENTE ---\n");
    if(!mysql_num_rows(r))printf("Este cliente ainda nao possui endereco cadastrado.\n");
    while((row=mysql_fetch_row(r)))printf("ID %s | %s, %s - %s, %s/%s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
    mysql_free_result(r);
}

static void cadastrarEndereco(void) {
    listarClientes(); int cliente=lerInteiro("ID do cliente: ");
    if(!cliente||!registroExiste("cliente","id_cliente",cliente)){printf("Cliente nao encontrado.\n");return;}
    char rua[121],numero[11],comp[61],bairro[61],cidade[61],uf[3],cep[9];
    char a[242],b[24],c[122],d[122],e[122],f[8],g[20],sql[1000];
    lerTexto("Logradouro: ",rua,sizeof(rua)); lerTexto("Numero: ",numero,sizeof(numero));
    lerTexto("Complemento (opcional): ",comp,sizeof(comp)); lerTexto("Bairro: ",bairro,sizeof(bairro));
    lerTexto("Cidade: ",cidade,sizeof(cidade)); lerTexto("UF (2 letras): ",uf,sizeof(uf)); lerTexto("CEP (8 numeros): ",cep,sizeof(cep));
    if(!rua[0]||!numero[0]||!bairro[0]||!cidade[0]||strlen(uf)!=2||strlen(cep)!=8){printf("Preencha os campos obrigatorios. UF deve ter 2 letras e CEP 8 digitos.\n");return;}
    escapar(rua,a,sizeof(a));escapar(numero,b,sizeof(b));escapar(comp,c,sizeof(c));escapar(bairro,d,sizeof(d));escapar(cidade,e,sizeof(e));escapar(uf,f,sizeof(f));escapar(cep,g,sizeof(g));
    if(comp[0])snprintf(sql,sizeof(sql),"INSERT INTO endereco (id_cliente,logradouro,numero,complemento,bairro,cidade,uf,cep) VALUES (%d,'%s','%s','%s','%s','%s','%s','%s')",cliente,a,b,c,d,e,f,g);
    else snprintf(sql,sizeof(sql),"INSERT INTO endereco (id_cliente,logradouro,numero,bairro,cidade,uf,cep) VALUES (%d,'%s','%s','%s','%s','%s','%s')",cliente,a,b,d,e,f,g);
    if(executarSQL(sql))printf("Endereco cadastrado! ID: %llu\n",mysql_insert_id(conexao));
}

static void listarPedidos(void) {
    MYSQL_RES *r=consultarSQL("SELECT p.id_pedido,c.nome,p.status,p.canal,p.forma_pagamento,p.valor_total,DATE_FORMAT(p.data_hora,'%d/%m/%Y %H:%i') FROM pedido p JOIN cliente c ON c.id_cliente=p.id_cliente ORDER BY p.data_hora DESC LIMIT 100");
    if(!r)return; MYSQL_ROW row; printf("\n--- PEDIDOS (ULTIMOS 100) ---\n");
    if(!mysql_num_rows(r))printf("Nenhum pedido cadastrado.\n");
    while((row=mysql_fetch_row(r)))printf("Pedido #%s | %-22s | %-12s | %-9s | %-12s | R$ %-9s | %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
    mysql_free_result(r);
}

static int obterProduto(int id, char *nome, size_t tamanho, double *preco) {
    char sql[300]; snprintf(sql,sizeof(sql),"SELECT nome,preco FROM produto WHERE id_produto=%d AND ativo=TRUE",id);
    MYSQL_RES *r=consultarSQL(sql); if(!r)return 0; MYSQL_ROW row=mysql_fetch_row(r);
    if(!row){mysql_free_result(r);return 0;}
    snprintf(nome,tamanho,"%s",row[0]); *preco=strtod(row[1],NULL); mysql_free_result(r); return 1;
}

static void registrarPedido(void) {
    ItemPedido itens[MAX_ITENS]; int total=0;
    listarClientes(); int cliente=lerInteiro("ID do cliente: ");
    if(!cliente||!registroExiste("cliente","id_cliente",cliente)){printf("Cliente nao encontrado.\n");return;}
    listarEnderecosCliente(cliente); int endereco=lerInteiro("ID do endereco de entrega: ");
    char sql[1200]; snprintf(sql,sizeof(sql),"SELECT 1 FROM endereco WHERE id_endereco=%d AND id_cliente=%d",endereco,cliente);
    MYSQL_RES *re=consultarSQL(sql); if(!re||!mysql_num_rows(re)){if(re)mysql_free_result(re);printf("Endereco nao pertence ao cliente ou nao existe.\n");return;} mysql_free_result(re);
    int usuario=lerInteiro("ID do usuario/atendente: ");
    if(!usuario||!registroExiste("usuario","id_usuario",usuario)){printf("Usuario nao encontrado.\n");return;}
    printf("Canal: 1-APP  2-SITE  3-WHATSAPP\n"); int canal=lerInteiro("Escolha: ");
    const char *canalSQL=canal==1?"APP":canal==2?"SITE":canal==3?"WHATSAPP":NULL;
    if(!canalSQL){printf("Canal invalido.\n");return;}
    char pagamento[21]; lerTexto("Forma de pagamento (PIX, DINHEIRO, CARTAO): ",pagamento,sizeof(pagamento));
    for(char *p=pagamento;*p;p++)*p=(char)toupper((unsigned char)*p);
    if(!pagamento[0]){printf("Forma de pagamento obrigatoria.\n");return;}
    listarProdutos();
    printf("\nAdicione produtos. Digite ID 0 para finalizar.\n");
    for(;;){
        int id=lerInteiro("ID do produto: "); if(id==0)break;
        char nome[101]; double preco;
        if(!obterProduto(id,nome,sizeof(nome),&preco)){printf("Produto nao encontrado ou inativo.\n");continue;}
        int qtd=lerInteiro("Quantidade: "); if(qtd<=0){printf("Quantidade deve ser maior que zero.\n");continue;}
        int pos=-1; for(int i=0;i<total;i++)if(itens[i].id_produto==id)pos=i;
        if(pos>=0)itens[pos].quantidade+=qtd;
        else if(total<MAX_ITENS){itens[total].id_produto=id;itens[total].quantidade=qtd;itens[total].preco=preco;snprintf(itens[total].nome,sizeof(itens[total].nome),"%s",nome);total++;}
        else {printf("Limite de %d produtos diferentes por pedido atingido.\n",MAX_ITENS);break;}
    }
    if(total==0){printf("Pedido cancelado: nenhum item foi adicionado.\n");return;}
    double soma=0; printf("\n--- RESUMO DO PEDIDO ---\n");
    for(int i=0;i<total;i++){double sub=itens[i].quantidade*itens[i].preco;soma+=sub;printf("%s | %d x R$ %.2f = R$ %.2f\n",itens[i].nome,itens[i].quantidade,itens[i].preco,sub);}
    printf("TOTAL: R$ %.2f\n",soma);
    char confirma[8];lerTexto("Confirmar pedido? (S/N): ",confirma,sizeof(confirma));
    if(toupper((unsigned char)confirma[0])!='S'){printf("Pedido cancelado.\n");return;}
    char pg[64];escapar(pagamento,pg,sizeof(pg));
    if(!executarSQL("START TRANSACTION"))return;
    snprintf(sql,sizeof(sql),"INSERT INTO pedido (id_cliente,id_endereco,id_usuario,canal,forma_pagamento,status,valor_total) VALUES (%d,%d,%d,'%s','%s','RECEBIDO',%.2f)",cliente,endereco,usuario,canalSQL,pg,soma);
    if(!executarSQL(sql)){executarSQL("ROLLBACK");return;}
    unsigned long long pedido=(unsigned long long)mysql_insert_id(conexao);
    for(int i=0;i<total;i++){
        snprintf(sql,sizeof(sql),"INSERT INTO item_pedido (id_pedido,id_produto,quantidade,preco_unitario) VALUES (%llu,%d,%d,%.2f)",pedido,itens[i].id_produto,itens[i].quantidade,itens[i].preco);
        if(!executarSQL(sql)){executarSQL("ROLLBACK");return;}
    }
    if(!executarSQL("COMMIT")){executarSQL("ROLLBACK");return;}
    printf("Pedido #%llu registrado com sucesso! Total: R$ %.2f\n",pedido,soma);
    printf("O estoque sera baixado automaticamente quando o pedido mudar para CONFIRMADO, conforme o trigger do banco.\n");
}

static void atualizarStatusPedido(void) {
    listarPedidos(); int id=lerInteiro("ID do pedido: ");
    if(!id||!registroExiste("pedido","id_pedido",id)){printf("Pedido nao encontrado.\n");return;}
    printf("Status disponiveis: RECEBIDO, CONFIRMADO, EM_PREPARO, EM_ROTA, ENTREGUE, CANCELADO\n");
    char status[21],s[48],sql[300]; lerTexto("Novo status: ",status,sizeof(status));
    for(char *p=status;*p;p++)*p=(char)toupper((unsigned char)*p);
    const char *validos[]={"RECEBIDO","CONFIRMADO","EM_PREPARO","EM_ROTA","ENTREGUE","CANCELADO"}; int ok=0;
    for(int i=0;i<6;i++)if(strcmp(status,validos[i])==0)ok=1;
    if(!ok){printf("Status invalido.\n");return;}
    escapar(status,s,sizeof(s)); snprintf(sql,sizeof(sql),"UPDATE pedido SET status='%s' WHERE id_pedido=%d",s,id);
    if(executarSQL(sql))printf("Status do pedido atualizado.\nSe foi alterado para CONFIRMADO, o trigger do MySQL tentou registrar a baixa dos insumos.\n");
}

static void detalhesPedido(void) {
    int id=lerInteiro("ID do pedido: "); char sql[500];
    snprintf(sql,sizeof(sql),"SELECT p.id_pedido,c.nome,p.status,p.canal,p.forma_pagamento,p.valor_total,DATE_FORMAT(p.data_hora,'%%d/%%m/%%Y %%H:%%i') FROM pedido p JOIN cliente c ON c.id_cliente=p.id_cliente WHERE p.id_pedido=%d",id);
    MYSQL_RES *r=consultarSQL(sql); if(!r)return; MYSQL_ROW row=mysql_fetch_row(r);
    if(!row){printf("Pedido nao encontrado.\n");mysql_free_result(r);return;}
    printf("\nPedido #%s | Cliente: %s | Status: %s | Canal: %s | Pagamento: %s | Total: R$ %s | Data: %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]); mysql_free_result(r);
    snprintf(sql,sizeof(sql),"SELECT pr.nome,i.quantidade,i.preco_unitario,(i.quantidade*i.preco_unitario) FROM item_pedido i JOIN produto pr ON pr.id_produto=i.id_produto WHERE i.id_pedido=%d",id);
    r=consultarSQL(sql);if(!r)return;printf("Itens:\n");
    while((row=mysql_fetch_row(r)))printf("- %s | %s un. x R$ %s = R$ %s\n",row[0],row[1],row[2],row[3]);mysql_free_result(r);
}

static void menuPedidos(void) {
    int op;
    do {
        printf("\n===== PEDIDOS =====\n1 - Registrar pedido\n2 - Listar pedidos\n3 - Consultar detalhes de pedido\n4 - Atualizar status\n0 - Voltar\n");
        op=lerInteiro("Opcao: ");
        switch(op){case 1:registrarPedido();break;case 2:listarPedidos();break;case 3:detalhesPedido();break;case 4:atualizarStatusPedido();break;case 0:break;default:printf("Opcao invalida.\n");}
        if(op)pausar();
    }while(op!=0);
}

/* ======================== RELATORIOS ======================== */
static void relatorioEstoqueBaixo(void) {
    MYSQL_RES *r=consultarSQL("SELECT nome,qtd_atual,unidade_medida,qtd_minima FROM insumo WHERE qtd_atual<qtd_minima ORDER BY nome");
    if(!r)return; MYSQL_ROW row;printf("\n--- INSUMOS ABAIXO DO MINIMO ---\n");
    if(!mysql_num_rows(r))printf("Nenhum insumo abaixo do estoque minimo.\n");
    while((row=mysql_fetch_row(r)))printf("%s | Atual: %s %s | Minimo: %s\n",row[0],row[1],row[2],row[3]);mysql_free_result(r);
}

static void relatorioFaturamento(void) {
    MYSQL_RES *r=consultarSQL("SELECT COUNT(*),COALESCE(SUM(valor_total),0) FROM pedido WHERE status<>'CANCELADO' AND data_hora>=NOW()-INTERVAL 30 DAY");
    if(!r)return;MYSQL_ROW row=mysql_fetch_row(r);if(row)printf("\nUltimos 30 dias: %s pedidos | Faturamento: R$ %s\n",row[0],row[1]);mysql_free_result(r);
}

static void menuRelatorios(void) {
    int op;do{printf("\n===== RELATORIOS =====\n1 - Insumos abaixo do minimo\n2 - Faturamento dos ultimos 30 dias\n3 - Produtos mais vendidos\n0 - Voltar\n");op=lerInteiro("Opcao: ");
        if(op==1)relatorioEstoqueBaixo();
        else if(op==2)relatorioFaturamento();
        else if(op==3){MYSQL_RES*r=consultarSQL("SELECT pr.nome,SUM(i.quantidade) total FROM item_pedido i JOIN produto pr ON pr.id_produto=i.id_produto JOIN pedido p ON p.id_pedido=i.id_pedido WHERE p.status<>'CANCELADO' GROUP BY pr.id_produto,pr.nome ORDER BY total DESC LIMIT 10");if(r){MYSQL_ROW row;printf("\n--- 10 PRODUTOS MAIS VENDIDOS ---\n");while((row=mysql_fetch_row(r)))printf("%-28s %s unidades\n",row[0],row[1]);mysql_free_result(r);}}
        else if(op!=0)printf("Opcao invalida.\n");if(op)pausar();
    }while(op!=0);
}

int main(void) {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8); SetConsoleOutputCP(CP_UTF8);
#endif
    char host[128]="localhost", usuario[128]="root", banco[128]="petisco_ja";
    const char *senha=getenv("PETISCO_DB_PASSWORD");
    const char *env=getenv("PETISCO_DB_HOST"); if(env&&*env)snprintf(host,sizeof(host),"%s",env);
    env=getenv("PETISCO_DB_USER"); if(env&&*env)snprintf(usuario,sizeof(usuario),"%s",env);
    env=getenv("PETISCO_DB_NAME"); if(env&&*env)snprintf(banco,sizeof(banco),"%s",env);
    if(!senha||!*senha){printf("Defina a variavel de ambiente PETISCO_DB_PASSWORD com a senha do MySQL antes de executar.\n");return 1;}
    conexao=mysql_init(NULL);
    if(!conexao){printf("Nao foi possivel iniciar o cliente MySQL.\n");return 1;}
    if(!mysql_real_connect(conexao,host,usuario,senha,banco,3306,NULL,0)){
        printf("Falha na conexao com MySQL: %s\n",mysql_error(conexao));mysql_close(conexao);return 1;
    }
    mysql_set_character_set(conexao,"utf8mb4");
    printf("\n========================================\n          PETISCO JA - SISTEMA\n========================================\nConectado ao banco '%s'.\n",banco);
    int op;
    do {
        printf("\n=============== MENU PRINCIPAL ===============\n");
        printf("1 - Gerenciar clientes\n2 - Gerenciar produtos\n3 - Gerenciar pedidos\n4 - Controle de estoque\n5 - Relatorios\n0 - Sair\n");
        op=lerInteiro("Escolha uma opcao: ");
        switch(op){case 1:menuClientes();break;case 2:menuProdutos();break;case 3:menuPedidos();break;case 4:menuEstoque();break;case 5:menuRelatorios();break;case 0:printf("Encerrando o Petisco Ja. Ate mais!\n");break;default:printf("Opcao invalida.\n");}
    }while(op!=0);
    mysql_close(conexao);
    return 0;
}
