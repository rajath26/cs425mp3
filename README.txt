This is the readme file with instructions to use the 'Daisy Distributed System In Memory Key Value Store'.

Step 1) Compile
Run the compile.sh script
$ ./compile.sh

Step 2) Start the node
Run the startDaisyFDNode.sh script in src folder.
This script has the following options:
-p : Port Number
-i : IP Address 
-t : Node type which can be: 
     1) "leader"
     2) "member"
-h : Host number
     Each host should be designated a number like '0' or '1' or any integer. When the node is brought down and if it has to join back (reincarnation) then the same host number must be provided 
$ ./startDaisyFDNode -p <port_no> -i <ip_address> -t <node_type> -h <host_num>

Step 3) Start several nodes

Step 4) Start client to execute INSTRUCTIONS ON KEY VALUE STORE. 
$ ./KVclient <port_no> <ip_address> <server_port_no_on_current_host> <instruction>

Below are examples:

$ ./KVclient 4091 192.17.11.79 4092 "INSERT:::1111:::TEST"
$ ./KVclient 4091 192.17.11.79 4092 "UPDATE:::1111:::TEST1"
$ ./KVclient 4091 192.17.11.79 4092 "LOOKUP:::1111"
$ ./KVclient 4091 192.17.11.79 4092 "DELETE:::1111"
