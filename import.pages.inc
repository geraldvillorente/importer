<?php
// ms_settings_form
function import_upload_file_form($form, &$form_state) {
  $form = array();
  $form['csv_file'] = array('#title' => t('Csv file') ,'#type' => 'managed_file', '#required' => TRUE);
  $form['zip_file'] = array('#title' => t('Images zip file') ,'#type' => 'managed_file', '#required' => TRUE);
  $form['csv_file']['#upload_validators']['file_validate_extensions'] = array('csv');
  $form['zip_file']['#upload_validators']['file_validate_extensions'] = array('zip');

  $form['submit'] = array('#type' => 'submit', '#value' => t('Process'));
  return $form;
}

function import_upload_file_form_submit($form, &$form_state) {

  $images = array();
  $rows = array();
  ini_set('auto_detect_line_endings', true);

  if ($csv_file = file_load($form_state['values']['csv_file'])) {
    if (($handle = fopen($csv_file->uri, "r")) !== FALSE) {
      while (($data = fgetcsv($handle, NULL, ",")) !== FALSE) {
        $rows[] = $data;
      }
    }
  }
  if ($zip_file = file_load($form_state['values']['zip_file'])) {
    module_load_include('inc', 'zip_archive', 'zip_archive.class');
    $zip = new ZipArchive;
    $path = drupal_realpath($zip_file->uri);
    $zip->open($path);
    $dir = "temporary://extracted_files";
    $zip->extractTo(drupal_realpath($dir));
    $files = file_scan_directory($dir, '/.*\.*$/');

    foreach ($files as $key => $value) {
      $images[$value->name] = $key;
    }
  }

  if ($rows) {
    //remove the header row from the array.
    array_shift($rows);
    _import_smartphones($rows, $images);
  }

}