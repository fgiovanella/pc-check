# 🖥️ Monitor de Processos

Este é um projeto desenvolvido para a disciplina de Sistemas Operacionais. Trata-se de um monitor de sistema gráfico, inspirado no Gerenciador de Tarefas do Windows, construído do zero em C++ com foco em eficiência e na aplicação direta de conceitos do SO.

O programa monitora em tempo real o uso de CPU e RAM, além de listar todos os processos e threads em execução no sistema.

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

Um dos principais desafios deste projeto era o risco de "saturar o processador". Um monitor de sistema ingênuo, que coleta todos esses dados 60 vezes por segundo (junto com a UI), se tornaria o programa mais pesado da máquina.

Este projeto resolveu esse problema de forma robusta, aplicando conceitos centrais de concorrência:

1. **Arquitetura Multi-Thread:** O programa é dividido em duas threads principais:
    * **Thread de UI (Rápida):** Responsável apenas por desenhar a interface (ImGui). Ela é "burra" e não faz trabalho pesado.
    * **Thread de Coleta (Lenta):** Roda em segundo plano (`std::thread`), executa todo o trabalho pesado (cálculos de delta de CPU, listagem de processos) e, em seguida, **"dorme" por 1 segundo** (`std::this_thread::sleep_for`).
2. **Sincronização Segura:** Um `std::mutex` é usado para proteger os dados. A thread de coleta trava o mutex, atualiza os dados, e o libera. A thread da UI trava o mutex brevemente apenas para ler esses dados, garantindo que não haja conflitos.
3. **Coleta Sob Demanda:** Dados pesados (como I/O de disco e detalhes de threads) não são coletados no loop principal. Eles são buscados *apenas* quando o usuário clica em um processo específico, garantindo performance máxima.
4. **Otimização de Cache:** `std::map` são usados para guardar os dados de CPU "antigos" de cada processo e thread, permitindo o cálculo do "delta" de uso.
5. **Otimização de Memória:** O loop da UI não cria novas cópias da lista de processos 60x por segundo. Ele reutiliza a mesma estrutura de dados (`SystemInfo& outInfo`), evitando a "agitação de memória" que causava o alto consumo de RAM.

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

## 🚀 Guia de Instalação e Execução

Siga este passo a passo para configurar, compilar e rodar o projeto em sua máquina.

### 1️⃣ Pré-requisitos (Configuração do Ambiente)

Antes de abrir o projeto, certifique-se de que o seu Visual Studio está pronto para código C++.

1. Abra o **Visual Studio Installer** (no Menu Iniciar do Windows).
2. Na lista, encontre sua versão do Visual Studio e clique em **Modificar**.
3. Na aba **"Cargas de Trabalho"** (*Workloads*), verifique se a seguinte opção está marcada:
   * ☑️ **Desenvolvimento para desktop com C++** (*Desktop development with C++*)
4. Se não estiver, marque-a e clique em **Modificar/Instalar** no canto inferior direito.



### 2️⃣ Abrindo o Projeto Corretamente

Este projeto utiliza **CMake**, o que significa que não existe um arquivo de solução (`.sln`) tradicional na pasta raiz.

1. Abra o **Visual Studio**.
2. Na tela de boas-vindas, clique na opção:
   * 📂 **Abrir uma pasta local** (*Open a local folder*)
3. Navegue até a pasta onde você baixou/extraiu o projeto (`PcCheck`) e clique em **Selecionar Pasta**.



> **Nota:** Aguarde alguns instantes após abrir. Observe a **Janela de Saída** (*Output*) na parte inferior. O Visual Studio irá configurar o CMake e baixar as bibliotecas necessárias (ImGui, GLFW) automaticamente. Espere aparecer a mensagem: *"Geração do CMake concluída"*.

### 3️⃣ Compilando para Alta Performance

Para garantir que o monitor mostre o uso real de memória (cerca de **80MB**) e não o uso inflado pelo depurador (1GB+), compilaremos em modo **Release**.

1. Na barra de ferramentas superior (topo da tela), localize o menu que diz `Debug`.
2. Altere de `Debug` para **`Release`**.
3. No menu superior, clique em:
   * **Compilação** > **Recriar Tudo** (*Build > Rebuild All*).
4. Aguarde a compilação terminar. Você deve ver: *"Êxito: 1"* na janela de saída.



### 4️⃣ Executando o Programa

Você tem duas opções para rodar o Monitor de Sistema:

**Opção A: O Jeito Rápido (Dentro do VS)**

1. Localize o botão verde ▶️ **"Selecionar Item de Inicialização"** na barra superior.
2. Selecione `MeuMonitor.exe` (ou `PcCheck.exe`).
3. Clique no botão verde para rodar.

**Opção B: O Jeito Profissional (Executável Independente)**

*Para ver a performance real do programa sem o peso do Visual Studio:*

1. Feche o Visual Studio (opcional, mas recomendado para testar isoladamente).
2. Abra o **Explorador de Arquivos** e entre na pasta do projeto.
3. Navegue pelo seguinte caminho:
   `out` ➝ `build` ➝ `x64-Release` ➝ `src`
4. Dê dois cliques no arquivo **`MeuMonitor.exe`**.

---

## 👥 Autores

* Felipe Giovanella
* Gustavo Campestrini
* Nicolas Ceruti
