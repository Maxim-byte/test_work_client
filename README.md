<h1>
Simple http async client, build on boost asio and boost beast
</h1>

<h2>
General Description
</h2>

1) To pull all dependencies do - cd conan && conan install conanfile.txt --build=missing
2) Log file create near binary file
3) Correct user turn off by sending SIGTERM
   <h2>
   Description common.json
   </h2>

Option  | Description
--------------------------------|---------------------------------
times_to_reconnect_s            | Time during which we try to reconnect
time_to_flush_logs_s            | Period for flushing logs
server_data.host                | Server(backend) ip address 
server_data.port                | Server(backend) port
logging.file_logger             | Name of file logger
logging.console_logger          | ame of console logger

