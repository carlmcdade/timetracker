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
            
			$('#update-mini-form').ajaxForm({target: this});
						
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
							$("#row-" + row[1]).remove();
							$('#total-user-hours').html(result);
						}
				});
				
				return false;
				
			});
			
			$("a.update-time").live("click", function(event) {
					
				event.preventDefault();	
	 			var row = $(this).attr("id").split('-');
 			
	 			$("#id-mini-form").val(row[1]);
	 			
	 			$('#row-' + row[1]).after($("#mini-form-row").show());
	 			
	 			$("#update-mini-form").show(); 
	 			
	 			$("#update-mini-form #edit-task").val($('.cell-2-' + row[1]).text());
	 			
	 			$("#update-mini-form #edit-event-date").val($('.cell-3-' + row[1]).text());
	 			
	 			$("#update-mini-form #edit-hours").val($('.cell-4-' + row[1]).text());
	 			
	 			
	 			
			});
			
			$("div.form-close").live("click", function(event) {
					$("tr:visible[id*='mini-form-']").hide();
			});
			
			$("#sendUpdate").live("click",function() {			
				$.ajax({
							type: "POST",
							dataType: "json",
							url: "http://drupal.se/timetracker/timetracker/update_time/",
							data: $('#update-mini-form').serialize(),
							cache: false,
							success: function(result){
								
								$.ajax({
									type: "POST",
									url: "http://drupal.se/timetracker/timetracker/myreport/",
									data: result,
									cache: false,
									success: function(html){
										
										$("#output").html(html);
									
									}
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
