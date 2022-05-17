



setwd(dirname(rstudioapi::getActiveDocumentContext()$path))

ns = c(10 ^ 6, 10 ^ 7, 10 ^ 8)
ms = c(2 * 10 ^ 6, 2 * 10 ^ 7, 2 * 10 ^ 8)
ps = c(2, 4, 8, 11, 16, 32)



job_outputs_path = "../parmerge-student-1.0.0-Source/job_outputs"
for (excercise_folder_name in list.files(path = job_outputs_path)) {
  # print(excercise_folder_name)
  excercise_folder_path = paste(job_outputs_path, excercise_folder_name, sep =
                                  "/")
  for (batch_folder_name in list.files(path = excercise_folder_path)) {
    # print(batch_folder_name)
    

    
    batch_folder_path = paste(excercise_folder_path, batch_folder_name, sep =
                                "/")
    
    for (i in 1:length(ns)) {
      file_start = sprintf("output_%s_%d_%d", excercise_folder_name, ns[i], ms[i])
      all_results_for_batch_with_size = matrix(ncol = 2, nrow = length(ps))
      colnames(all_results_for_batch_with_size) = c("processors", "time")
      
      
      j = 1
      for (p in ps) {
        trial_file_name = sprintf("%s_%d.out", file_start, p)
        
        
        
        full_file_path = paste(batch_folder_path, trial_file_name, sep = "/")
        print(full_file_path)
        trial_results = read.csv(full_file_path)
        
        processors = trial_results[1, "p"]
        avg_time = mean(trial_results[, "time"])
        
        all_results_for_batch_with_size[j, 1] = processors
        all_results_for_batch_with_size[j, 2] = avg_time
        
        j = j + 1
        
      }
      png(file= sprintf("./plot_%s_%s_%d_%d.png",excercise_folder_name, batch_folder_name, ns[i], ms[i]) ,width=1960/2, height=1080/2)
      plot(type="l", xaxt="no",  all_results_for_batch_with_size, main=sprintf("%s: n = %s, m = %s", excercise_folder_name, format(ns[i],big.mark=",",scientific=FALSE), format(ms[i],big.mark=",",scientific=FALSE)))
      axis(1, at = ps)
      dev.off()
    }
    
    
  }

}
