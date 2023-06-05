#$version='7.5.116.383-HEAD-85cb2f8'


$login_username='GitAction'
$login_password='iOpEv21kPOt5ZrMqViLKvJJXshh6olNR'
$login_api='/api/auth/login'
$login_url='http://download.weixitianli.com'+$login_api + '?Username='+$login_username+'&Password='+$login_password
$token=(Invoke-RestMethod -Uri $login_url -Method Post).data.token
Write-Host $token

Write-Host  上传

$upload_api='/api/fs/put'
$upload_url='http://download.weixitianli.com'+$upload_api
$upload_file='cvAutoTrack-'+$version+'.zip'
$upload_file_length=(Get-Item $upload_file).Length
$upload_path='/sync/TianLiUpdateService.tianyi/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}

$upload_api='/api/fs/put'
$upload_url='http://download.weixitianli.com'+$upload_api
$upload_file='cvAutoTrack-'+$version+'.zip'
$upload_file_length=(Get-Item $upload_file).Length
$upload_path='/sync/TianLiUpdateService.object/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}

$upload_api='/api/fs/put'
$upload_url='http://download.weixitianli.com'+$upload_api
$upload_file='cvAutoTrack-'+$version+'.zip'
$upload_file_length=(Get-Item $upload_file).Length
$upload_path='/sync/TianLiUpdateService.local/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}
try{
    $uploadInfo = Invoke-RestMethod -Uri $upload_url -Method Put -Headers $upload_headers -InFile $upload_file
    Write-Host $uploadInfo
    return 0
}
catch{
	Write-Host "Upload Error, Program Stop."
    return 504
}


