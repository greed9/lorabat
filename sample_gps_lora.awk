BEGIN {
	FS=","
	SAMPLE_INTERVAL = 10
	readArduinoCmd = "~/lora_gps/readArduinoData.sh"
}

/GPVTG/ {
	# groundspeed in kilometers per hour from GPS converted to MPH
	speedGPS = $8 * 0.621371
	#printf( "speedGPS=%f\n", speedGPS ) 
 
}

/GPGGA/ {
	count ++
	if( count > SAMPLE_INTERVAL ) {

		# gawk idiom to shell a command and catch result
		# This one gets rssi readings from the LoRa radio via the Arduino
		# Kicks off the python wrapper around serial
                # port access.  Don't forget -- to avoid Arduino reset
                # on serial port open, stty -F /dev/ttyUSB0 -hupcl
                #

       		readArduinoCmd | getline sampledData
       		close(readArduinoCmd)

		# Format of data from server Arduino:
		#
		# millis_local,millis_remote,seq_number,client_rssi,marker_num,server_rssi
		#
		# marker_num is populated only when the marker button has been pressed recently.
		#

		# Try to stay in sync by discarding first line read
		#printf( "read from Arduino: %s\n", sampledData )
		split( sampledData, fields, "," )
		millis = fields[1]
		seq = fields[2]
		client_rssi = fields[3]
		marker_num = fields[4]
		server_rssi = fields[5]
		#printf( "millis=%d, seq=%d, client_rssi=%d, marker_num=%d\n", millis, seq, client_rssi, marker_num ) ;

		# GPS altitude in feet
		gpsAltitude = $10 * 3.28084 

		# GPS time
		gpsTime = $2

		# NEMA GPS lat/lon to decimal degrees, from Stack Overflow
		# lat first.  NEMA is ddmm.mmmm
		deg = substr( $3, 1,  2) * 1.0
		min = substr( $3, 3,  7) * 1.0
		lat = deg + ( min / 60.0 )
		if( $4 == "S") {
			lat = lat * -1.0
		}

		# Now long, NEMA is dddmm.mmmm
		deg = substr( $5, 1, 3 ) * 1.0
		min = substr( $5, 4, 7 ) * 1.0
		lon = deg + ( min / 60.0 )
		if( $6 == "W") {
			lon = lon * -1.0
		}

		# Save merged data to stdout
		#
		# Format is:
		#
		#  gpstime,millis,seq,lat,long,altitude,client_rssi,marker_num
		#
		printf( "%06d,%d,%d,%f,%f,%f,%d,%d,%d\n", gpsTime, millis, seq, lat, lon, gpsAltitude, client_rssi, marker_num, server_rssi )
		fflush( "/dev/stdout")
		count = 0
	}
}

