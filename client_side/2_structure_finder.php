<?php

	header("Content-Type: text/plain");

	include_once "0_prepare_connection.php";

	$protein_id = $_GET["protein_id"];

	$structure_filter_string = $_GET["structure_prefix"];
	$structure_filter_string_percent = $structure_filter_string . "%";

	$sequence_list = null;

	try {
		$sequence_list = $libalotuser_connects_database->query("select STRUCTURE_ID, STRUCTURE_DATABANK_ID, STRUCTURE_FILE_TYPE from TB_STRUCTURE where STRUCTURE_PROTEIN_ID = $protein_id and STRUCTURE_DATABANK_ID like '$structure_filter_string_percent' order by STRUCTURE_DATABANK_ID");
	} catch (PDOException $e) {
		echo "Error in execution: " . $e->getMessage() . "\n";
		exit;
	}

	$result = array();

	foreach ($sequence_list as $row) {
		$result[] = array(
							'STRUCTURE_ID' => $row['STRUCTURE_ID'],
							'STRUCTURE_DATABANK_ID' => $row['STRUCTURE_DATABANK_ID'],
							'STRUCTURE_FILE_TYPE' => $row['STRUCTURE_FILE_TYPE']
						);
	}

	echo json_encode( $result );

?>
