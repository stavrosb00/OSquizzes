#lab2_exe se fish
function sort_files -a file_extension year flag -description="sort_files(file_extension,year,flag)"
    mkdir -p output
    if test $flag = "n"
        find ./ -type f -newermt $year-01-01 -name "*.$file_extension" | xargs -I '{}' mv '{}' output
    end
    if test $flag = "o"
        find ./ -type f -not -newermt $year-01-01 -name "*.$file_extension" | xargs -I '{}' mv '{}' output
    end
end 









function sort_files
switch $argv[3]
    case o
        find ./ -name "*.$argv[1]" ! -newermt "$argv[2]/01/01"|xargs -I {} mv '{}' ./output
    case n
        find ./ -name "*.$argv[1]" -newermt "$argv[2]/01/01"|xargs -I {} mv '{}' ./output
    end
end