activaAmpleDeBanda 
    * No hay parametros

activaEmmascarament
    * No hay parametros

modificarAccesPerSubxarxa P1 P2
    * P1 -> 1 for DROP
         -> 2 for MASQUERADE
    * P2 -> from 1 to 8
         -> 1: 192.168.2.32
         -> 2: 192.168.2.64
         -> ...
         -> 8: 192.168.2.224

canviarBalanceig P2 P2
    * P1 -> Entero (ETH0)
    * P2 -> Entero (ETH1)
    * Proporcion de carga

modificarAmpleDeBandaPerSubxarxa P1 P2 P3
    * P1 -> ethX (0 - 1)
    * P2 -> numero de regla (2 - 8)
    * P3 -> velocidad

resetAmpleDeBandaGateway P1 P2
    * P1 -> 0 para eth0
         -> 1 para eth1
    * P2 -> velocidad  
