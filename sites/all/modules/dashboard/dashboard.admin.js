if(Drupal.jsEnabled) {
  $(document).ready(function(){
    Drupal.CTools.AJAX.commands.modal_dismiss = null;
    Drupal.CTools.AJAX.commands.modal_dismiss = function(){
      Drupal.CTools.Modal.dismiss();
      window.location.reload(true);
    }
  });

  /**
   * Default behaviors - add default to default list.
   */
  Drupal.behaviors.dashboardMakeAvailable = function(context) {
    $('a.make-available-default').click(function() {

      var default_subtype_value = $(this).attr('id').replace(/^make-available-default-/, '');
      var link_element = this;

      // Save the data into database
      $.post(Drupal.settings.basePath + 'admin/settings/dashboard/default-options/make-available',
        {
          subtype: default_subtype_value 
        },
        function(data, textStatus, XMLHttpRequest) {
          $(link_element).replaceWith(data.label);
        },
        "json"
      );

      return false;
    });
  }

  /**
   * Default behaviors - Remove default to default list.
   */
  Drupal.behaviors.dashboardRemove = function(context) {
    $('a.remove-default').click(function() {

      var default_subtype_value = $(this).attr('id').replace(/^remove-default-/, '');
      var link_element = this;

      // Save the data into database
      $.post(Drupal.settings.basePath + 'admin/settings/dashboard/default-options/remove',
        {
          subtype: default_subtype_value 
        },
        function(data, textStatus, XMLHttpRequest) {
          $(link_element).replaceWith(data.label);
        },
        "json"
      );

      return false;
    });
  }

}
