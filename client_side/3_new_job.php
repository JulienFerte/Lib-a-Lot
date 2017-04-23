<?php

	header("Content-Type: text/plain");

	include_once "0_prepare_connection.php";

	$sequence_id = $_GET["sequence_id"];
	$mutation = $_GET["mutation"];
	$structure_id = $_GET["structure_id"];

//*** Parsing then rebuilding the mutation string, need to check for good formating
//*** parsing the mutation input
	preg_match_all('/(\d+)(\w)/', $mutation, $matches);

	$number_of_mutations = count($matches[0]);

	$mutation_array = array();
	for( $i = 0; $i < $number_of_mutations; $i++) {
		$mutation_array[$i] = array( 'offset' => $matches[1][$i], 'residue' => $matches[2][$i] );
	}
	usort( $mutation_array, "compare_point_mutations" );

//*** Gets the mutation id, creates it if necessary
//*** Creates the string of offsets and residues
	$mutation_string = "";
	for( $i = 0; $i < $number_of_mutations; $i++ ) {
		$mutation_string = $mutation_string . $mutation_array[$i]['offset'] . $mutation_array[$i]['residue'];
	}

	try {
		$mutation_list = $libalotuser_connects_database->query("select MUTATION_ID from VW_MUTATION where MUTATION_OFFSETS_RESIDUES = '$mutation_string' limit 1");
	} catch (PDOException $e) {
		echo "Error in execution: " . $e->getMessage() . "\n";
		exit;
	}

	$mutation_id = 0;

	if( $mutation_list->rowCount() > 0 ) {
		$row = $mutation_list->fetch();
		$mutation_id = $row["MUTATION_ID"];
	}
	else {
		$libalotuser_connects_database->exec("insert into TB_MUTATION(MUTATION_SEQUENCE_ID) values ($sequence_id)");
		$mutation_id = $libalotuser_connects_database->lastInsertId();
		$insert_point_mutations = "insert into TB_POINT_MUTATION(POINT_MUTATION_MUTATION_ID,POINT_MUTATION_OFFSET,POINT_MUTATION_RESIDUE) values ";
		$point_mutations_array = array();
		for( $i = 0; $i < $number_of_mutations; $i++) {
			$point_mutations_array[$i] = "( " . $mutation_id . ", " . $mutation_array[$i]["offset"] . ", '" . $mutation_array[$i]["residue"] . "' )";
		}
		$insert_point_mutations .= implode( ",", $point_mutations_array );
		$libalotuser_connects_database->exec($insert_point_mutations);
	}

//*** Sends the tasks
//*** one TCP connection for each packet, one packet for each task
	error_reporting(E_ALL);

//*** Allow the script to hang around waiting for connections
	set_time_limit(0);

//*** Turn on implicit output flushing so we see what we're getting as it comes in
	ob_implicit_flush();

//*** Ask for computations on the mutated sequence
	for( $i = 0; $i < count( $modules ); $i++ ) {

		if(($socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
			echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
		}

		if(socket_connect($socket, $engine_ip, intval( $engine_port )) === false) {
			echo "socket_connect() failed: reason: " . socket_strerror(socket_last_error($socket)) . "\n";
		}

		$message = "" . $i . " " . $sequence_id . " " . $mutation_id . " " . $structure_id;

		socket_write($socket, $message, strlen($message));

		socket_close($socket);

//*** Wait half a second
usleep( 500000 );
	}

//*** Ask for computations on the wild-type sequence
	for( $i = 0; $i < count( $modules ); $i++ ) {

		if(($socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
			echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
		}

		if(socket_connect($socket, $engine_ip, intval( $engine_port )) === false) {
			echo "socket_connect() failed: reason: " . socket_strerror(socket_last_error($socket)) . "\n";
		}

		$message = "" . $i . " " . $sequence_id . " 0 " . $structure_id;

		socket_write($socket, $message, strlen($message));

		socket_close($socket);

//*** Wait half a second
usleep( 500000 );
	}

//*** Create new job
	$pdo_uuid = $libalotuser_connects_database->query("select replace(UUID(),'-','') as UUID");
	$row = $pdo_uuid->fetch();
	$job_uuid = $row["UUID"];

	$job_creation = "insert into TB_JOB(JOB_UUID,JOB_SEQUENCE_ID,JOB_MUTATION_ID,JOB_STRUCTURE_ID) values (unhex('$job_uuid'),"
						. $sequence_id . ","
						. ( $mutation_id == 0 ? "NULL" : "" . $mutation_id ) . ","
						. ( $structure_id == 0 ? "NULL" : "" . $structure_id ) . ")";

	$libalotuser_connects_database->exec($job_creation);

//*** Echo the uuid of the job
	echo $job_uuid;

?>
