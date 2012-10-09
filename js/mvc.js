(function ($) {		

  Drupal.behaviors.mvc = {
    attach: function(context, settings) {      
     $(document).ready(function() {
     		 
     	// container start
     	   
            $('#timetracker-mytime-').ajaxForm({
            success: function(result){
            	
					var d = new Date();
	
					var month = d.getMonth()+1;
					var day = d.getDate();
					
					var output = d.getFullYear() + '-' +
						((''+month).length<2 ? '0' : '') + month + '-' +
						((''+day).length<2 ? '0' : '') + day;
						
					var section = result.split('}{');
					
					// show any previously hidden tables
					//$('#table-').show();
					$("div:hidden[id*='table-']").show();
					
					// hide the working table 
					// $('#table-' + section[0]).hide();
					$('div#most-recent-events table > tbody').prepend(section[1]);
					
					// remove the last row to keep the list short ( 5 rows )
					$('div#most-recent-events table > tbody > tr:last').remove();
					
					
					$('div#table-' + section[0] + ' table > thead').append(section[1]);
					
					
					
					// show new working table
					/*$('#output').html(section[1]).css({
							backgroundColor: '#99FF66',
							padding: '10px'
							});*/
;
                }
             
            });
            
            $('#timetracker-myreports-').ajaxForm({
            target:'#output',
            success: function(){
            	            $('#export-form').show();
							$("#my-report-id").val($('#edit-my-projects-id option:selected').val());
						}
            });
            
			$('#update-mini-form').ajaxForm({
			target: this});
						
			$( "#edit-event-date" ).AnyTime_picker(
				  { format: "%Y-%m-%d",
					formatUtcOffset: "%: (%@)",
					placement: "popup" });
			
	 
	 		$("a.delete-time").live("click", function(event) {
	 			
	 			event.preventDefault();	
	 			var row = $(this).attr("id").split('-');
	 			var delurl = $(this).attr("href");
								
				$.ajax({
						type: "GET",
						url: delurl,
						dataType: "html",
						cache: false,
						success: function(result){
							
							section = result.split('}{');
							
							$("#row-" + row[1]).fadeOut('slow', function(){
									$(this).remove();							
							});
							
							
							$('#total-row-' + section[0] + ' #total-user-hours').text(section[1]);
						
							
						}
				});
				
				return false;
				
			});
			
			$("a.update-time").live("click", function(event) {
					
				event.preventDefault();	
				
	 			var row = $(this).attr("id").split('-');			
	 			
	 			$("tr:visible[id*='form-row-']").hide();
	 			
	 			console.log($('input#edit-task-' + row[1]).val($('row-' + row[1]).text()));
	 			
	 			/// reset the sect list to the selected value if not changed by submit
	 			$("#update-entry-" + row[1]).each (function() { this.reset(); });
	 			
	 			$("#form-row-" + row[1]).addClass('temp-form').show('slow');
	 			
			});
			
			$("a.update-recent-time").live("click", function(event) {
					
				event.preventDefault();	
				
	 			var row = $(this).attr("id").split('-');			
	 			
	 			$("tr:visible[id*='recent-form-row-']").hide();
	 			
	 			$('input#edit-task-' + row[1]).val($('recent-' + row[1]).text());
	 			
	 			/// reset the sect list to the selected value if not changed by submit
	 			$("#update-entry-" + row[1]).each (function() { this.reset(); });
	 			
	 			$("#recent-form-row-" + row[1]).addClass('temp-form').show('slow');
	 			
			});
			
			$("div.form-close").live("click", function(event) {
					$("tr:visible[class*='temp-form']").hide('slow');
			});
			
			$("#sendUpdate").live("click",function() {
					
				var row = $(this).parents().eq(6).attr('id').split('-');
				$.ajax({
							type: "POST",
							dataType: "html",
							url: "http://drupal.se/timetracker/timetracker/update_time/",
							data:{
								id:$('input#form-id-' + row[2]).val(),
								task:$('input#edit-task-' + row[2]).val(),
								hours:$('input#edit-hours-' + row[2]).val(),
								project:$('select#projects-' + row[2]).val(),
							},
							cache: false,
							success: function(result){
								
								section = result.split('}{');
								$('#row-' + section[0]).fadeOut("slow", function(){

								$(this).replaceWith(section[1]);
								
								$('#row-' + section[0]).fadeIn("slow");
								
								$('#total-row-' + section[3] + ' #total-user-hours').text(section[2]);
								$('#total-row-' + section[3]).removeClass().addClass('event-added');
								

								});
								
							}
				});
				
				
				return false;			
				
			});
			
			$('#edit-export').live("click", function(){
				
					$('#timetracker-myreports-csv-').attr("action", 'http://drupal.se/timetracker/timetracker/myreport_csv/');
				
					$('#timetracker-myreports-csv-').submit();
				
			});
						

		  // container end
	  });
    }
  };

})(jQuery);
