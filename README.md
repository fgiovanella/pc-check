# 🖥️ Monitor de Processos

Este é um projeto desenvolvido para a disciplina de Sistemas Operacionais. Trata-se de um monitor de sistema gráfico, inspirado no Gerenciador de Tarefas do Windows, construído do zero em C++ com foco em eficiência e na aplicação direta de conceitos do SO.

O programa monitora em tempo real o uso de CPU e RAM, além de listar todos os processos e threads em execução no sistema.

![Screenshot do PcCheck em execução](DEU_CERTO.png)
*(Sugestão: Tire um print da sua aplicação funcionando, salve o arquivo como `DEU_CERTO.png` na pasta raiz do projeto para esta imagem aparecer)*

---

## ✨ Funcionalidades Principais

* **Dashboard Geral:** Barras de progresso que exibem o uso total de **RAM** e **CPU** do sistema.
* **Lista de Processos Detalhada:** Uma tabela completa com todos os processos em execução, incluindo:
    * PID (ID do Processo)
    * Nome
    * Uso de CPU (%)
    * Uso de Memória (RSS)
* **Interatividade:**
    * **Encerrar Tarefa:** Um botão "Encerrar" em cada processo para forçar o seu término.
    * **Painel de Foco:** Ao clicar no PID de um processo, um painel de detalhes é exibido.
* **Painel de Detalhes (Por Processo):**
    * **Diagnóstico de I/O:** Mostra o total de bytes lidos e escritos no disco.
    * **Análise de Threads:** Lista todas as threads individuais do processo focado e o **uso de CPU de cada thread**.

---

## 🛠️ O Desafio de Eficiência (O "Buraco Mais Embaixo")

Um dos principais desafios deste projeto, era o risco de "saturar o processador". Um monitor de sistema ingênuo, que coleta todos esses dados 60 vezes por segundo (junto com a UI), se tornaria o programa mais pesado da máquina.

Este projeto resolveu esse problema de forma robusta, aplicando conceitos centrais de concorrência:

1.  **Arquitetura Multi-Thread:** O programa é dividido em duas threads principais:
    * **Thread de UI (Rápida):** Responsável apenas por desenhar a interface (ImGui). Ela é "burra" e não faz trabalho pesado.
    * **Thread de Coleta (Lenta):** Roda em segundo plano (`std::thread`), executa todo o trabalho pesado (cálculos de delta de CPU, listagem de processos) e, em seguida, **"dorme" por 1 segundo** (`std::this_thread::sleep_for`).
2.  **Sincronização Segura:** Um `std::mutex` é usado para proteger os dados. A thread de coleta trava o mutex, atualiza os dados, e o libera. A thread da UI trava o mutex brevemente apenas para ler esses dados, garantindo que não haja conflitos.
3.  **Coleta Sob Demanda:** Dados pesados (como I/O de disco e detalhes de threads) não são coletados no loop principal. Eles são buscados *apenas* quando o usuário clica em um processo específico, garantindo performance máxima.
4.  **Otimização de Cache:** `std::map` são usados para guardar os dados de CPU "antigos" de cada processo e thread, permitindo o cálculo do "delta" de uso.
5.  **Otimização de Memória:** O loop da UI não cria novas cópias da lista de processos 60x por segundo. Ele reutiliza a mesma estrutura de dados (`SystemInfo& outInfo`), evitando a "agitação de memória" que causava o alto consumo de RAM.

---

## 🚀 Conceitos de SO Aplicados

* **Gerenciamento de Processos:** `EnumProcesses`, `OpenProcess`, `GetModuleBaseName`, `TerminateProcess`.
* **Gerenciamento de Memória:** `GlobalMemoryStatusEx`, `GetProcessMemoryInfo`.
* **Gerenciamento de CPU (Escalonamento):**
    * Cálculo de Carga Total (`GetSystemTimes`).
    * Cálculo de Carga por Processo (`GetProcessTimes`).
    * Cálculo de Carga por Thread (`GetThreadTimes`).
* **Concorrência e Sincronização:** `std::thread`, `std::mutex`, `std::atomic`.
* **Interação com API de Baixo Nível:** Uso direto da Win32 API.
* **Recursos do SO:** `GetProcessHandleCount`, `GetProcessIoCounters`, `GetPriorityClass`.
* **Gerenciamento de Threads:** `CreateToolhelp32Snapshot` (com `TH32CS_SNAPTHREAD`), `Thread32First`/`Thread32Next`.

---

## 💻 Tecnologias Utilizadas

* **Linguagem:** C++17
* **Sistema de Build:** CMake
* **Interface Gráfica (GUI):** [Dear ImGui](https://github.com/ocornut/imgui)
* **Janela e Contexto Gráfico:** [GLFW](https://www.glfw.org/)
* **APIs do Sistema:** Win32 API (`windows.h`, `psapi.h`, `pdh.h`, `tlhelp32.h`)

---

## 🔧 Como Compilar e Executar (Visual Studio IDE)

Este projeto é configurado com CMake e deve ser compilado usando o **Visual Studio 2019/2022**.

**Pré-requisitos:**

1.  Instale o **Visual Studio Community** (ou outra versão).
2.  Durante a instalação, na aba "Cargas de Trabalho", certifique-se de marcar a opção **"Desenvolvimento para desktop com C++"**.

**Passos para Compilação:**

1.  **Não** use o terminal (PowerShell) para compilar. Deixe o IDE fazer todo o trabalho.
2.  Abra o **Visual Studio IDE**.
3.  Na tela inicial, escolha **"Abrir uma pasta local"** (NÃO "Abrir projeto ou solução").
4.  Selecione a pasta raiz `PcCheck` (a que contém o `CMakeLists.txt`).
5.  Aguarde o Visual Studio configurar o CMake automaticamente (observe a "Janela de Saída").
6.  Na barra de ferramentas superior, mude a configuração de "Debug" para **"Release"**. (Isso garante que o programa rode leve, com ~80MB de RAM, em vez de 1GB+).
7.  Vá ao menu superior e clique em:
    **`Compilação -> Recarregar Tudo`** (Build -> Rebuild All).
8.  Clique no botão verde (▶️) de "Executar" para rodar o programa (anexado ao depurador).

**Para rodar o `.exe` final e leve (Recomendado):**
1.  Após compilar em modo **Release**, feche o Visual Studio.
2.  Navegue no Windows Explorer até a pasta: `out/build/x64-Release/src/`
3.  Execute o `MeuMonitor.exe` diretamente.

---

## 👥 Autores

* Felipe Giovanella
* Gustavo Campestrini
* Nicolas Ceruti
