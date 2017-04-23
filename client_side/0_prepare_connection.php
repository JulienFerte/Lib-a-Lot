<?php

	$modules = array();

	$config = simplexml_load_file( "../.LibALot.conf" );

	foreach( $config->children() as $node ) {
		if( $node->getName() == "servers" ) {
			foreach( $node->children() as $node2 ) {
				if( $node2->getName() == "mysql" ) {

					foreach( $node2->attributes() as $key => $value ) {
						if( $key == "ip" ) {
							$address = $value;
						}
						if( $key == "port" ) {
							$port = $value;
						}
						if( $key == "database" ) {
							$database = $value;
						}
					}

					foreach( $node2->children() as $node3 ) {
						if( $node3->getName() == "client" ) {
							foreach( $node3->attributes() as $key => $value ) {
								if( $key == "login" ) {
									$login = $value;
								}
								if( $key == "password" ) {
									$password = $value;
								}
							}
						}
					}
				}

				if( $node2->getName() == "core" ) {

					foreach( $node2->attributes() as $key => $value ) {
						if( $key == "ip" ) {
							$engine_ip = $value;
						}
						if( $key == "port" ) {
							$engine_port = $value;
						}
					}
				}
			}
		}
		if( $node->getName() == "modulelist" ) {
			foreach( $node->children() as $node2 ) {
				foreach( $node2->attributes() as $key => $value ) {
					if( $key == "name" ) {
						array_push( $modules, $value );
					}
				}
			}
		}
	}

	try {
		$libalotuser_connects_database = new PDO("mysql:host=$address;dbname=$database", $login, $password);
		$libalotuser_connects_database->setAttribute( PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION );
	}
	catch(PDOException $e) {
		echo "libalotuser_connects_database failed: " . $e->getMessage() . "\n";
	}

	function compare_point_mutations($a, $b)
	{
		if ($a['offset'] == $b['offset']) {
		    return 0;
		}
		return ($a['offset'] < $b['offset']) ? -1 : 1;
	}

?>
