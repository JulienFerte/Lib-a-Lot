<?php

	header("Content-Type: text/plain");

	include_once "0_prepare_connection.php";

	$sequence_filter_string = $_GET["sequence_prefix"];
	$sequence_filter_string_percent = $sequence_filter_string . "%";

	$sequence_list = null;

	try {
		if( preg_match( "/[0-9]/", $sequence_filter_string ) ) {
			$sequence_list = $libalotuser_connects_database->query("SELECT PROTEIN_ID, PROTEIN_NAME, PROTEIN_ENTRY_NAME, PROTEIN_IS_WILD_TYPE, SEQUENCE_ID, SEQUENCE_RESIDUES, SEQUENCE_UNIPROT_ID, SEQUENCE_ISOFORM_NUMBER, SEQUENCE_ISOFORM_NAME FROM VW_PROTEIN_SEQUENCE WHERE SEQUENCE_UNIPROT_ID LIKE '$sequence_filter_string_percent' ORDER BY SEQUENCE_UNIPROT_ID");
		}
		else {
			$sequence_list = $libalotuser_connects_database->query("SELECT PROTEIN_ID, PROTEIN_NAME, PROTEIN_ENTRY_NAME, PROTEIN_IS_WILD_TYPE, SEQUENCE_ID, SEQUENCE_RESIDUES, SEQUENCE_UNIPROT_ID, SEQUENCE_ISOFORM_NUMBER, SEQUENCE_ISOFORM_NAME FROM VW_PROTEIN_SEQUENCE WHERE SEQUENCE_RESIDUES LIKE '$sequence_filter_string_percent' ORDER BY SEQUENCE_RESIDUES");
		}
	} catch ( PDOException $e ) {
		echo "Error in execution: " . $e->getMessage() . "\n";
		exit;
	}

	$result = array();

	foreach( $sequence_list as $row ) {
		$result[] = array(
							'PROTEIN_ID' => ($row['PROTEIN_ID']+0),
							'PROTEIN_NAME' => $row['PROTEIN_NAME'],
							'PROTEIN_ENTRY_NAME' => $row['PROTEIN_ENTRY_NAME'],
							'PROTEIN_IS_WILD_TYPE' => ($row['PROTEIN_IS_WILD_TYPE']+0),
							'SEQUENCE_ID' => ($row['SEQUENCE_ID']+0),
							'SEQUENCE_RESIDUES' => $row['SEQUENCE_RESIDUES'],
							'SEQUENCE_UNIPROT_ID' => $row['SEQUENCE_UNIPROT_ID'],
							'SEQUENCE_ISOFORM_NUMBER' => ($row['SEQUENCE_ISOFORM_NUMBER']+0),
							'SEQUENCE_ISOFORM_NAME' => $row['SEQUENCE_ISOFORM_NAME']
						);
	}

	echo json_encode( $result );

?>
