# 🍔 Petisco Já

Sistema de gerenciamento para uma lanchonete, desenvolvido em **linguagem C** com integração ao **MySQL**.

O projeto tem como objetivo facilitar o gerenciamento de clientes, produtos, pedidos e estoque, centralizando as principais operações de uma lanchonete em um único sistema.

## 🚀 Funcionalidades

### 👥 Cadastro de clientes
- Cadastrar novos clientes.
- Consultar clientes cadastrados.
- Atualizar informações.
- Excluir clientes.
- Gerenciar endereços.

### 🍟 Gerenciamento de produtos
- Cadastrar produtos.
- Consultar produtos disponíveis.
- Atualizar informações dos produtos.
- Excluir produtos.
- Organizar produtos por categorias.

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
- Relacionar produtos aos seus respectivos insumos.
- Definir a quantidade de cada insumo utilizada na produção.
- Controlar o consumo de materiais.

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
- Biblioteca MariaDB Connector/C
- Visual Studio Code
- GCC (MinGW/MSYS2)

## ⚙️ Como executar o projeto

### 1. Pré-requisitos

Antes de executar, é necessário ter:

- Windows.
- MySQL 8 instalado e configurado.
- GCC instalado.
- MariaDB Connector/C configurado.
- Banco de dados `petisco_ja` criado e estruturado.

### 2. Configurar a senha do banco

No CMD do Windows, configure a variável de ambiente:

```cmd
set PETISCO_DB_PASSWORD=sua_senha
```

Substitua `sua_senha` pela senha do seu MySQL.

### 3. Compilar o projeto

Abra o projeto no Visual Studio Code e pressione:

`Ctrl + Shift + B`

Selecione a tarefa de compilação configurada para gerar o arquivo `main.exe`.

### 4. Executar pelo CMD

Abra o Prompt de Comando na pasta do projeto e execute:

```cmd
main.exe
```

### 5. Executar automaticamente

Para facilitar a inicialização, é possível utilizar um arquivo `executar.bat` na mesma pasta do `main.exe`.

Conteúdo do arquivo:

```bat
@echo off
title Petisco Ja
cd /d "%~dp0"
main.exe
pause
```

Assim, basta abrir o `executar.bat` para iniciar o sistema pelo CMD.

## 📁 Estrutura do projeto

```text
petisco-ja/
│
├── main.c
├── main.exe
├── executar.bat
├── README.md
│
└── .vscode/
    └── tasks.json
```

*Os arquivos e diretórios podem variar conforme a configuração local do projeto.*

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

**Victor Gasparotti**

Estudante de Análise e Desenvolvimento de Sistemas.

GitHub: [@victorgasparotti](https://github.com/victorgasparotti)

---

**Petisco Já — Gerenciando sua lanchonete de forma simples e organizada!** 🍔