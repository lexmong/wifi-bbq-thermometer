var app = angular.module('app', ['ngRoute']);

app.config(function ($routeProvider, $locationProvider) {
    $routeProvider.when('/', {
        controller: 'tempCtrl',
        templateUrl: '/probes.html',
    });
});

app.controller('tempCtrl', ['$scope', '$http', function ($scope, $http) {
    $scope.unit = 'c';

    $scope.probes = [
        {id:1,target:'-',temperature:-1},
        {id:2,target:'-',temperature:-1},
        {id:3,target:'-',temperature:-1},
        {id:4,target:'-',temperature:-1}
    ]

    function getTemp() {
        $http.get('/read?temp=1?unit=c')
            .then((response) => {
                console.log(response);
                for(let i =0;i<4;i++){
                    $scope.probes[i].temperature = Math.round(response.data[i]);
                }
            }, (response) => {
                console.log(response);
                for(let i =0;i<4;i++){
                    $scope.probes[i].temperature =-1;
                }
            });
    }
    
    getTemp();

    let interval = setInterval(function() {
        getTemp();
    }, 2000);

}]);
