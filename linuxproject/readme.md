# 智能监控

## epoll

- **新的客户端连接**：当一个新的客户端尝试连接到服务器时，这被视为一个事件。我们的服务器需要响应这个事件，接受这个新的连接，并将新的 socket 添加到 `epoll` 的监视列表中。
- **客户端发送数据**：当已连接的客户端发送数据到服务器时，这也被视为一个事件。服务器需要读取并处理这些数据。在我们的示例中，服务器简单地将接收到的数据回发给客户端，但在实际应用中，你可能需要进行更复杂的处理。
- **客户端断开连接**：当客户端关闭其连接或当从客户端 socket 读取到 0 字节时，这意味着客户端已断开连接。这也被视为一个事件，服务器需要响应这个事件，关闭这个 socket，并进行任何必要的清理。

## sockets

- **创建服务器 socket (`server_fd`)**：这是服务器的主要 socket，用于监听来自客户端的连接请求。

- **接受新的客户端连接**：当客户端尝试连接到服务器时，我们使用 `accept()` 函数来接受这个连接。这会返回一个新的 socket (`new_socket`)，代表与客户端的连接。

- **读取和发送数据**：对于每个已连接的客户端，我们使用其对应的 socket 来读取从客户端发送来的数据，并发送数据回客户端。在示例中，我们简单地将接收到的数据回发给客户端。

- **关闭连接**：当客户端关闭其连接或当我们从客户端 socket 读取到 0 字节时，我们使用 `close()` 函数关闭这个 socket。

- **添加到 `epoll` 监视**：为了使用 `epoll` 高效地处理多个并发连接，我们需要将 `server_fd` 和所有新的客户端 sockets (`new_socket`) 添加到 `epoll` 的监视列表中。

## 声音模块

**声音传感器模块**：这些模块通常有一个模拟输出，表示环境中的声音级别。树莓派没有模拟输入，所以你需要一个模拟到数字转换器（ADC）如MCP3008来读取这些模块。

- 将MCP3008的VDD和VREF引脚连接到3.3V。
- 将MCP3008的AGND和DGND引脚连接到GND。
- 将MCP3008的CLK、DOUT、DIN和CS/SHDN引脚分别连接到树莓派的SCLK、MISO、MOSI和CE0引脚。
- 将声音传感器的模拟输出连接到MCP3008的CH0引脚（或任何其他通道）