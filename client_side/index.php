<!DOCTYPE html>
<html>

	<head>
		<meta charset="utf-8">
		<!--[if lt IE 9]>
			<script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>
		<![endif]-->
		<link rel="stylesheet" href="style.css" />
		<script type="text/javascript" src="scripts.js"></script>
		<!--[if lte IE 7]>
			<link rel="stylesheet" href="style_ie.css" />
		<![endif]-->
		<title>
			Welcome to Lib-A-Lot
		</title>
	</head>

	<body>

		<input id="id_input_sequence" type="text" value=""
				style="text-transform:uppercase"
				onkeypress="filterNonAlphanumeric( event )"
				onkeyup="fillWithFilteredSequences()"
				/>

		<span id="id_span_sequence_finder" >
		</span>


		<input id="id_input_mutation_offset" type="text" value=""
				onkeypress="filterSequenceOffset( event )"
				onkeyup="fillWithResidues()"
				/>

		<div id="id_div_choice_mutated_residue"
			>
		</div>

		<div id="id_div_chosen_mutation" type="text"
			>
		</div>


		<div id="id_div_chosen_structure" type="text"
			>
		</div>

		<span id="id_span_structure_finder" >
		</span>


		<button id="id_button_send_task"
				onclick="sendTask()"
				>
			Send job
		</button>

	</body>

</html>
