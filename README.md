# 🖥️ Monitor de Sistema (Projeto de S.O.)

Este é um projeto desenvolvido para a disciplina de Sistemas Operacionais. Trata-se de um monitor de sistema gráfico, inspirado no Gerenciador de Tarefas do Windows, capaz de exibir o consumo de recursos (CPU, RAM) e gerenciar os processos em execução em tempo real.

O projeto é focado na plataforma Windows, fazendo uso direto da Win32 API para coletar as informações do sistema.

---

## ✨ Funcionalidades Principais

* **Monitoramento de CPU:** Exibe a carga total de uso da CPU em uma barra de progresso.
* **Monitoramento de RAM:** Exibe o uso de memória RAM (Total e Usada) em uma barra de progresso.
* **Lista de Processos Detalhada:** Apresenta uma tabela com todos os processos em execução no sistema.
* **Detalhes por Processo:** Para cada processo, exibe:
    * **PID** (ID do Processo)
    * **Nome** do executável
    * **Uso de CPU (%)**
    * **Uso de Memória (RSS)**
* **Gerenciamento de Tarefas:** Permite **"Encerrar Tarefa"**, finalizando processos selecionados diretamente pela interface.
* **Arquitetura Eficiente:** Utiliza **multi-threading** para separar a coleta de dados (backend) da renderização da interface (frontend). Isso garante que a UI permaneça fluida (alto FPS) sem ser travada pela coleta de dados, que é uma operação mais lenta.

---

## 💻 Tecnologias Utilizadas

* **Linguagem:** C++17
* **Sistema de Build:** CMake
* **Interface Gráfica (GUI):** [Dear ImGui](https://github.com/ocornut/imgui) (v1.89.9)
* **Janela e Contexto Gráfico:** [GLFW](https://www.glfw.org/)
* **APIs do Sistema:** Win32 API (`windows.h`, `psapi.h`, `pdh.h`)
* **Concorrência:** `std::thread`, `std::mutex`, `std::atomic`

---

## 🔧 Como Compilar e Executar (Visual Studio IDE)

Este projeto é configurado com CMake e é melhor compilado usando o **Visual Studio 2019/2022**.

**Pré-requisitos:**

1.  Instale o **Visual Studio Community** (ou outra versão).
2.  Durante a instalação, na aba "Cargas de Trabalho", certifique-se de marcar a opção **"Desenvolvimento para desktop com C++"**. Isso é essencial, pois instala o compilador C++ e as ferramentas CMake.

**Passos para Compilação:**

1.  **Não** use o terminal ou o "Developer PowerShell". Deixe o IDE fazer todo o trabalho.
2.  Abra o **Visual Studio IDE**.
3.  Na tela inicial, escolha **"Abrir uma pasta local"** (NÃO "Abrir projeto ou solução").
4.  Selecione a pasta raiz do projeto (a pasta que contém o `CMakeLists.txt` e a pasta `src`).
5.  Aguarde alguns segundos. O Visual Studio detectará o `CMakeLists.txt` e configurará o projeto automaticamente na "Janela de Saída". (Isso baixará as bibliotecas ImGui e GLFW pela primeira vez).
6.  Após a "Geração do CMake" ser concluída, vá ao menu superior e clique em:
    **`Compilação -> Recarregar Tudo`** (Build -> Rebuild All).
7.  Se a compilação for bem-sucedida, o botão verde (▶) de "Executar" na barra de ferramentas ficará disponível, mostrando o nome `MeuMonitor.exe`.
8.  Clique no botão verde (▶) para rodar o programa.

---

## 👥 Autores

* Felipe Giovanella
* Gustavo Campestrini
* Nicolas Ceruti
