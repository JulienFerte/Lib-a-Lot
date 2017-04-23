<?php

	header("Content-Type: text/plain");

	include_once "0_prepare_connection.php";

	$job_uuid = $_GET["job_uuid"];

	$job_result = $libalotuser_connects_database->query(
									"select * from VW_JOB"
										. " where JOB_UUID = '" . $job_uuid . "'"
										. " limit 1");

	$results = array();

	if( !empty($job_result) && $job_result->rowCount() > 0 ) {
		$row = $job_result->fetch();

		$sequence_id = $row["SEQUENCE_ID"];

		$results["mutated_type"] = array();
		$results["wild_type"] = array();

//*** Module1 results
		if( $row["MODULE1_ID"] != null ) {
			$results["mutated_type"]["module1"] = array(
															"result_a" => $row["MODULE1_RESULT_A"],
															"result_b" => $row["MODULE1_RESULT_B"]
														);
		}

//*** Module1 wild-type results
		$wild_type_module1_result = $libalotuser_connects_database->query( "select * from TB_MODULE1 where MODULE1_SEQUENCE_ID=" . $sequence_id . " and MODULE1_MUTATION_ID is null limit 1" );
		if( !empty($wild_type_module1_result) && $wild_type_module1_result->rowCount() > 0 ) {
			$row_module1 = $wild_type_module1_result->fetch();

			$results["wild_type"]["module1"] = array(
														"result_a" => $row_module1["MODULE1_RESULT_A"],
														"result_b" => $row_module1["MODULE1_RESULT_B"]
													);
		}

//*** Module2 results
		if( $row["MODULE2_ID"] != null ) {
			$results["mutated_type"]["module2"] = array(
															"result_a" => $row["MODULE2_RESULT_A"],
															"result_b" => $row["MODULE2_RESULT_B"]
														);
		}

//*** Module2 wild-type results
		$wild_type_module2_result = $libalotuser_connects_database->query( "select * from TB_MODULE2 where MODULE2_SEQUENCE_ID=" . $sequence_id . " and MODULE2_MUTATION_ID is null limit 1" );
		if( !empty($wild_type_module2_result) && $wild_type_module2_result->rowCount() > 0 ) {
			$row_module2 = $wild_type_module2_result->fetch();

			$results["wild_type"]["module2"] = array(
														"result_a" => $row_module2["MODULE2_RESULT_A"],
														"result_b" => $row_module2["MODULE2_RESULT_B"]
													);
		}
	}

	echo json_encode( $results );

?>
