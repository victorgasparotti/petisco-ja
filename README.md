# 🥗 Petisco Já

Sistema de gerenciamento para um negócio de alimentação saudável, desenvolvido em **linguagem C** com integração ao **MySQL**.

O projeto tem como objetivo facilitar o gerenciamento de clientes, produtos, pedidos e estoque, auxiliando na organização das operações de um estabelecimento especializado em comidas saudáveis.

## 🚀 Funcionalidades

### 👥 Cadastro de clientes
- Cadastrar novos clientes.
- Consultar clientes cadastrados.
- Atualizar informações.
- Excluir clientes.
- Gerenciar endereços.

### 🥑 Gerenciamento de produtos
- Cadastrar refeições e alimentos saudáveis.
- Consultar produtos disponíveis.
- Atualizar informações dos produtos.
- Excluir produtos.
- Organizar produtos por categorias.

Exemplos de produtos:
- Saladas.
- Bowls saudáveis.
- Refeições fitness.
- Sucos naturais.
- Sanduíches naturais.
- Sobremesas saudáveis.

### 🛒 Gerenciamento de pedidos
- Registrar pedidos.
- Adicionar produtos aos pedidos.
- Calcular o valor total.
- Controlar o status dos pedidos.
- Consultar informações e histórico de pedidos.

### 📦 Controle de estoque
- Cadastro e gerenciamento de insumos.
- Registro de entradas e saídas.
- Consulta de movimentações.
- Identificação de estoque baixo.
- Baixa automática de estoque conforme a confirmação dos pedidos.

### 📋 Ficha técnica
- Relacionar produtos aos seus respectivos ingredientes.
- Definir a quantidade de cada ingrediente utilizada na preparação.
- Controlar o consumo de matérias-primas.

### 📊 Relatórios
- Relatórios de faturamento.
- Consulta de vendas.
- Acompanhamento do estoque.
- Identificação de produtos com estoque baixo.

## 🗄️ Banco de dados

O sistema utiliza **MySQL 8**, com o banco de dados chamado `petisco_ja`.

A modelagem contempla 13 tabelas:

- `perfil`
- `usuario`
- `cliente`
- `endereco`
- `categoria`
- `produto`
- `insumo`
- `produto_insumo`
- `pedido`
- `item_pedido`
- `movimentacao_estoque`
- `entregador`
- `entrega`

Também são utilizados relacionamentos entre tabelas, transações e trigger para automatizar operações relacionadas ao estoque.

## 🛠️ Tecnologias utilizadas

- Linguagem C
- MySQL 8
- MariaDB Connector/C
- Visual Studio Code
- GCC (MinGW/MSYS2)

## ⚙️ Como executar o projeto

### 1. Pré-requisitos

- Windows.
- MySQL 8 instalado e configurado.
- GCC instalado.
- MariaDB Connector/C configurado.
- Banco de dados `petisco_ja` criado e estruturado.

### 2. Configurar a senha do banco

No CMD do Windows:

```cmd
set PETISCO_DB_PASSWORD=sua_senha
```

Substitua `sua_senha` pela senha do seu MySQL.

### 3. Compilar o projeto

Abra o projeto no Visual Studio Code e pressione `Ctrl + Shift + B`, selecionando a tarefa de compilação configurada.

### 4. Executar pelo CMD

Na pasta do projeto, execute:

```cmd
main.exe
```

### 5. Inicialização automática

Também é possível utilizar o arquivo `executar.bat`, localizado na mesma pasta do `main.exe`, para iniciar o programa automaticamente pelo CMD.

## 🎯 Objetivo acadêmico

O Petisco Já faz parte de um projeto acadêmico (PIM) do curso de Análise e Desenvolvimento de Sistemas.

O desenvolvimento busca aplicar conhecimentos de:

- Lógica de programação.
- Linguagem C.
- Estruturas de dados.
- Banco de dados relacionais.
- Comandos SQL.
- Integração entre aplicação e banco de dados.
- Organização e desenvolvimento de sistemas.

## 👨‍💻 Desenvolvedor

**Victor Gasparotti** e **João Vitor Alves de Souza**

Estudantes de Análise e Desenvolvimento de Sistemas.

GitHub: [@victorgasparotti](https://github.com/victorgasparotti)

---

**Petisco Já — Praticidade e organização para uma alimentação mais saudável!** 🥗