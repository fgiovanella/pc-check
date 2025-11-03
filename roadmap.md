
## 🚀 Melhorias Técnicas e de Robustez (Backend)

| Categoria | Melhoria Sugerida | Detalhe Técnico |
| :--- | :--- | :--- |
| **Coleta de CPU** | **Uso da API PDH (Performance Data Helper)** | A API PDH (que você importou, mas não usou) é a maneira recomendada pela Microsoft para obter dados de performance em tempo real. É mais robusta e precisa para CPU e pode simplificar o cálculo do delta de tempo que você faz manualmente com `GetSystemTimes`. |
| **Gerenciamento de Processos** | **Melhor Tratamento de Permissões** | A chamada `OpenProcess` pode falhar por motivos de permissão (processos críticos do sistema). Você está tratando isso com o nome `<acesso negado>`, mas poderia tentar abrir o processo com um nível de acesso menor, se `PROCESS_QUERY_INFORMATION | PROCESS_VM_READ` falhar, para pelo menos obter o nome. |
| **Encerrar Tarefa** | **Tratamento de Erros e Confirmação** | Na função `killProcess`, adicione um retorno de erro ou use `GetLastError()` para saber se o processo foi realmente encerrado. Na UI, adicione uma caixa de diálogo de confirmação (`ImGui::OpenPopup`) antes de chamar `TerminateProcess`. |
| **Thread Safety** | **Usar `std::shared_ptr` com Mutex** | Para dados de sistema que mudam com pouca frequência (como nomes de processos), considere usar `std::shared_ptr` para passar dados entre threads sem cópia total, e use um `std::atomic_flag` para sinalizar que a estrutura `SystemInfo` foi atualizada. |
| **Otimização de String** | **Usar a WCHAR nativa** | Seu código converte `TCHAR` para `std::string` na coleta do nome do processo. Manter strings como `std::wstring` internamente (e converter apenas para exibição) pode ser mais nativo no Windows, dependendo da sua configuração de projeto. |

---

## 💻 Melhorias de Funcionalidade e UI (Frontend)

| Categoria | Melhoria Sugerida | Detalhe de Funcionalidade |
| :--- | :--- | :--- |
| **Classificação de Processos** | **Opções de Ordenação Dinâmica** | Atualmente, o código ordena apenas por memória. Adicione funcionalidade para **clicar no cabeçalho da tabela** ("PID", "Nome", "CPU %", "Memória") para reordenar a lista de processos (crescente/decrescente). |
| **Detalhes do Processo** | **Visualização Expandida (Subjanela)** | Ao clicar duas vezes em um processo, abra uma nova janela ImGui para mostrar detalhes adicionais: *Caminho Completo do Executável*, *Usuário* que o iniciou, *Contagem de Threads*, e *Tempo Total de Execução*. |
| **Filtro/Busca** | **Campo de Busca na Lista** | Adicione um `ImGui::InputText` acima da tabela para permitir que o usuário filtre a lista de processos pelo nome. |
| **Gráficos Históricos** | **Gráficos de Linha** | Adicione um pequeno gráfico de linha na seção de CPU e RAM para mostrar o histórico de uso nos últimos 60 segundos (um gráfico simples usando a API ImGui Plotting). |
| **Unidades de Memória** | **Formato Inteligente** | Seu código usa "MB" e "GB", mas poderia implementar uma função que automaticamente exibe B, KB, MB ou GB, tornando a exibição mais limpa para processos com uso muito baixo de memória. |

---

## 🔧 Melhorias de Build e Configuração

| Categoria | Melhoria Sugerida | Detalhe de Implementação |
| :--- | :--- | :--- |
| **CMake** | **Estrutura de Multi-plataforma** | Mesmo que você não vá rodar no Mac, você pode usar as variáveis do CMake (`if(WIN32)`, `if(APPLE)`) para estruturar seu código para suportar futuramente implementações diferentes para o `backend.cpp`, tornando-o um projeto "Cross-Platform Ready". |
| **Configuração de Build** | **Configuração de Linkagem** | Mova o `#pragma comment(lib, "pdh.lib")` para o `CMakeLists.txt` usando o comando `target_link_libraries()`. Isso é o padrão do CMake e é preferível ao uso de pragmas no código-fonte. |

