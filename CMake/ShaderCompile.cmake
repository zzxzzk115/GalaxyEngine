function(CompileShaders shader_dir output_dir)
    if (NOT EXISTS ${output_dir})
        file(MAKE_DIRECTORY ${output_dir})
    endif ()

    file(GLOB_RECURSE shader_files "${shader_dir}/*.frag" "${shader_dir}/*.vert")
    
    foreach (shader_file ${shader_files})
        get_filename_component(shader_name ${shader_file} NAME)
        set(output_file "${output_dir}/${shader_name}.spv")
        
        execute_process(
            COMMAND glslangValidator -V "${shader_file}" -o "${output_file}"
            RESULT_VARIABLE shader_compile_result
        )
        
        list(APPEND compiled_shaders ${output_file})
    endforeach ()

    if (shader_compile_result)
        message(FATAL_ERROR "Shader compilation failed!")
    else ()
        message("Shader compilation success!")
    endif ()
    
    set(${output_dir} ${compiled_shaders} PARENT_SCOPE)
endfunction ()