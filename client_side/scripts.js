//*** XMLHttpRequest global variable
var xhr;

function getXMLHttpRequest() {
	var xhr = null;

	if( window.XMLHttpRequest || window.ActiveXObject ) {
		if( window.ActiveXObject ) {
			try {
				xhr = new ActiveXObject( "Msxml2.XMLHTTP" );
			} catch(e) {
				xhr = new ActiveXObject( "Microsoft.XMLHTTP" );
			}
		} else {
			xhr = new XMLHttpRequest(); 
		}
	} else {
		alert( "Your browser doesn't have any XMLHTTPRequest object." );
		return null;
	}

	if( xhr && xhr.readyState != 0 ) {
		xhr.abort();
	}

	return xhr;
}

//*** Event juggling
function cancelEvent( event ) {
	event.preventDefault();
}

//*** Proteomics
var chosen_protein_id = 0;
var chosen_sequence_id = 0;
var chosen_sequence_residues = '';
var chosen_residue_offset = 0;
var wild_residue = '';
var chosen_mutated_residue = '';
var chosen_structure_id = 0;

//*** About the sequence
function filterNonAlphanumeric( event ) {
	var keyCode = event.which || event.keyCode;
	var character = String.fromCharCode( keyCode );
	var pattern = /[a-zA-Z0-9]/;

	if( !pattern.test( character ) && keyCode != 8 && keyCode != 13 && keyCode != 16 && keyCode != 17 && keyCode != 18 && keyCode != 37 && keyCode != 38 && keyCode != 39 && keyCode != 40 ) {
		event.preventDefault();
		return false;
	}
}

function emptySequenceDropDown() {
	var sequence_span = document.getElementById( "id_span_sequence_finder" );
	while( sequence_span.firstChild ) {
		sequence_span.removeChild( sequence_span.firstChild );
	}
}

function fillWithFilteredSequences() {
	sequence = document.getElementById( 'id_input_sequence' ).value.toUpperCase();

	if( sequence.length >= 1 ) {
		xhr = getXMLHttpRequest();

		xhr.onreadystatechange = function() {
//alert( '' + xhr.readyState + ' ' + xhr.status );
			if( xhr.readyState == 4 && xhr.status == 200 ) {
				sequences = JSON.parse( xhr.responseText );

				emptySequenceDropDown();

				var list = document.createElement( "ul" );
				list.className += " cl_sequence_dropdown";

				for( var i = 0; i < sequences.length; i++ ) {
					var element = document.createElement( "li" );
					element.className += " cl_select_sequence";

					element.id = "sequence_element_" + sequences[ i ][ 'SEQUENCE_ID' ];

					element.setAttribute( 'sequence_id', sequences[ i ][ 'SEQUENCE_ID' ] );
					element.setAttribute( 'sequence_residues', sequences[ i ][ 'SEQUENCE_RESIDUES' ] );
					element.setAttribute( 'protein_id', sequences[ i ][ 'PROTEIN_ID' ] );

					element.addEventListener( 'click', function( event ) { sequenceChoice( this ); } );

					var uniprot_id = sequences[ i ][ 'SEQUENCE_ISOFORM_NUMBER' ];
					element.innerHTML = '<span class="cl_protein_entry cl_font_bold">' + sequences[ i ][ 'PROTEIN_ENTRY_NAME' ] + '</span> '
										+ '<span class="cl_protein_name">' + sequences[ i ][ 'PROTEIN_NAME' ] + '</span> '
										+ '<span class="cl_sequence_uniprot_id cl_font_bold">' + sequences[ i ][ 'SEQUENCE_UNIPROT_ID' ] + ( uniprot_id == 0 ? '' : '-' + uniprot_id ) + '</span> '
										+ '<span class="cl_sequence_residues">' + sequences[ i ][ 'SEQUENCE_RESIDUES' ].substr( 0, 15 )+ '...' + '</span>';

					list.appendChild( element );
				}
				document.getElementById( "id_span_sequence_finder" ).appendChild( list );
			}
		};

		xhr.open( "GET", "1_sequence_finder.php?sequence_prefix=" + encodeURIComponent( sequence ) );
		xhr.send();
	}
}

function sequenceChoice( element ) {
	document.getElementById( 'id_input_sequence' ).value = element.getAttribute( 'sequence_residues' );

	chosen_protein_id = element.getAttribute( 'protein_id' );
	chosen_sequence_id = element.getAttribute( 'sequence_id' );
	chosen_sequence_residues = element.getAttribute( 'sequence_residues' );

	emptySequenceDropDown();

	fillStructures();
}

//*** About the mutation
function setChosenMutatedResidue( element ) {
	chosen_mutated_residue = element.getAttribute( 'mutated_residue' );

	document.getElementById( 'id_div_chosen_mutation' ).innerHTML = wild_residue + '' + chosen_residue_offset + chosen_mutated_residue;
}

function fillWithResidues() {
	var thing = Number( document.getElementById( 'id_input_mutation_offset' ).value );

	var mutation_choice_div = document.getElementById( 'id_div_choice_mutated_residue' );
	while( mutation_choice_div.firstChild ) {
		mutation_choice_div.removeChild( mutation_choice_div.firstChild );
	}

	if( thing <= 0 || chosen_sequence_residues.length < thing ) {
		return false;
	}

	chosen_residue_offset = thing;
	wild_residue = chosen_sequence_residues.charAt( chosen_residue_offset );

	var list_residues = [ "A", "C", "D", "E", "F", "G", "H", "I", "K", "L", "M", "N", "P", "Q", "R", "S", "T", "V", "W", "Y" ];

	var list = document.createElement( "ul" );

	for( var i = 0; i < list_residues.length; i++ ) {
		if( list_residues[ i ] != wild_residue ) {
			var element = document.createElement( "li" );
			element.className += " cl_mutated_residue cl_display_inline";

			element.setAttribute( 'mutated_residue', list_residues[ i ] );

			element.addEventListener( 'click', function( event ) { setChosenMutatedResidue( this ); } );

			element.innerHTML = list_residues[ i ];

			list.appendChild( element );
		}
	}

	document.getElementById( 'id_div_choice_mutated_residue' ).appendChild( list );
}

function filterSequenceOffset( event ) {
	var keyCode = event.which || event.keyCode;
	var character = String.fromCharCode( keyCode );
	var pattern = /[0-9]/;

	if( chosen_sequence_residues == '' || ( !pattern.test( character ) && keyCode != 8 && keyCode != 13 && keyCode != 16 && keyCode != 17 && keyCode != 18 && keyCode != 37 && keyCode != 38 && keyCode != 39 && keyCode != 40 ) ) {
		event.preventDefault();
		return false;
	}
}

//*** About the structure
function emptyStructureDropDown() {
	var structure_span = document.getElementById( "id_span_structure_finder" );
	while( structure_span.firstChild ) {
		structure_span.removeChild( structure_span.firstChild );
	}
}

function fillStructures() {
	xhr = getXMLHttpRequest();

	xhr.onreadystatechange = function() {
//alert( '' + xhr.readyState + ' ' + xhr.status );
		if( xhr.readyState == 4 && xhr.status == 200 ) {
			structures = JSON.parse( xhr.responseText );

			emptyStructureDropDown();

			var list = document.createElement( "ul" );
			list.className += " cl_structure_dropdown";

			for( var i = 0; i < structures.length; i++ ) {
				var element = document.createElement( "li" );
				element.className += " cl_select_structure";

				element.id = "structure_element_" + structures[ i ][ 'STRUCTURE_DATABANK_ID' ] + '_' + structures[ i ][ 'STRUCTURE_FILE_TYPE' ];

				element.setAttribute( 'structure_id', structures[ i ][ 'STRUCTURE_ID' ] );
				element.setAttribute( 'structure_descriptor', structures[ i ][ 'STRUCTURE_DATABANK_ID' ] + '.' + structures[ i ][ 'STRUCTURE_FILE_TYPE' ] );

				element.addEventListener( 'click', function( event ) { structureChoice( this ); } );

				element.innerHTML = '<span class="cl_structure_databank_id cl_font_bold">' + structures[ i ][ 'STRUCTURE_DATABANK_ID' ] + '</span> '
									+ '<span class="cl_structure_file_type">' + structures[ i ][ 'STRUCTURE_FILE_TYPE' ] + '</span>';

				list.appendChild( element );
			}
			document.getElementById( "id_span_structure_finder" ).appendChild( list );
		}
	};

	xhr.open( "GET", "2_structure_finder.php?protein_id=" + chosen_protein_id );
	xhr.send();
}

function structureChoice( element ) {
	chosen_structure_id = element.getAttribute( 'structure_id' );

	document.getElementById( 'id_div_chosen_structure' ).innerHTML = element.getAttribute( 'structure_descriptor' );

	emptyStructureDropDown();
}

//*** About the task
function sendTask() {
	xhr = getXMLHttpRequest();
	xhr.onreadystatechange = function() {
//alert( '' + xhr.readyState + ' ' + xhr.status );
		if( xhr.readyState == 4 && xhr.status == 200 ) {
			var job_uuid = xhr.responseText;




		}
	};

	xhr.open("GET", "3_new_job.php?"
						+ "sequence_id=" + document.getElementById('id_input_sequence').getAttribute('chosen_sequence_id')
						+ "&mutation=" + "0"
						+ "&structure_id=" + "0");
		xhr.send();
}

